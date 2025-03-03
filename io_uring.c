#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <liburing.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#include "kvs_server.h"


//#define __INIT_PORT 9000    //port of server work
#define ENTRIES_LENGTH 1024   //the length of queue

#define IO_URING_ACCEPT 0
#define IO_URING_RECV 1
#define IO_URING_SEND 2

#define ENABLE_EPOLL 0

struct conn_info{
    int fd;
    int event;
};

static int init_sock(unsigned short port){
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in s_addr;
    socklen_t s_len = sizeof(s_addr);
    bzero(&s_addr,s_len);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    int ret = bind(sockfd,(struct sockaddr *)&s_addr,s_len);
    if(ret == -1){
        perror("bind error:");
        exit(1);
    }
    ret = listen(sockfd, 128);
    if (ret == -1)
    {
        perror("listen error:");
        exit(1);
    }
    return sockfd;
}
int set_event_accept(struct io_uring *ring, int sockfd,struct sockaddr *addr,
					socklen_t *addrlen, int flags){
    bzero(addr,*addrlen);    //YOU WEN TI
    //memset(addr,0,*addrlen);
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_accept(sqe,sockfd,addr,addrlen,flags);
    
    struct conn_info listen_info = {
        .fd = sockfd,
        .event = IO_URING_ACCEPT,
    };
    memcpy(&sqe->user_data,&listen_info,sizeof(listen_info));

}

int set_event_recv(struct io_uring *ring,int sockfd,void *buf,
                    size_t buf_len,int flags){
    struct conn_info client_info= {
        .fd = sockfd,
        .event = IO_URING_RECV,
    };
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_recv(sqe,sockfd,buf,buf_len,flags);
    memcpy(&sqe->user_data,&client_info,sizeof(client_info));
    return sockfd;
}

int set_event_send(struct io_uring *ring,int sockfd,void *buf,
                    size_t buf_len,int flags){
    struct conn_info client_info= {
        .fd = sockfd,
        .event = IO_URING_SEND,
    };
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_send(sqe,sockfd,buf,buf_len,flags);
    memcpy(&sqe->user_data,&client_info,sizeof(client_info));
    return sockfd;
}

int kvs_io_uring_start(unsigned short port){
    //if(args != 2) return 0;
    //unsigned short port = atoi(argv[1]);
    int ret = 0;
    char buf[BUFSIZ] = {0};
    int sockfd = init_sock(port);
    //printf("init_sock successfuly\n");

    struct io_uring_params params;
    memset(&params,0,sizeof(params));

    struct io_uring ring;
    io_uring_queue_init_params(ENTRIES_LENGTH,&ring,&params);
    if(ret == -1){
        perror("io_uring_queue_init_params error:");
        exit(1);
    }

    //struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);
    set_event_accept(&ring,sockfd,(struct sockaddr *)&c_addr,&c_len,0);

#if ENABLE_EPOLL
    int epfd = epoll_create(1024);
    struct epoll_event *ev;
    ev->events = EPOLLIN;
    ev->data.fd = sockfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,ev);
#endif

    while(1){
        memset(buf,0,sizeof(buf));
        io_uring_submit(&ring);

        struct io_uring_cqe *cqe;
        io_uring_wait_cqe(&ring,&cqe);

        struct io_uring_cqe *cqes[128];
        int nread = io_uring_peek_batch_cqe(&ring,cqes,128);
        //printf("nread = %d\n",nread);
        for(int i = 0; i < nread ; i++){
            
            //printf("io_uring_peek_batch_cqe successfuly \n");
            struct io_uring_cqe *entries = cqes[i];
            struct conn_info resault;
            memcpy(&resault,&cqes[i]->user_data,sizeof(struct conn_info));
            if(resault.event == IO_URING_ACCEPT){
                set_event_accept(&ring,sockfd,(struct sockaddr *)&c_addr,&c_len,0);
                //printf("client have connected\n");
                int clientfd = entries->res;
                set_event_recv(&ring,clientfd,buf,sizeof(buf),0);
            }
            else if (resault.event == IO_URING_RECV)
            {
                int ret = entries->res;
                if(ret == 0){
                    //printf("recv:%d client %d have closed\n",ret,resault.fd);
                    close(resault.fd);
                }else if(ret > 0){
                    //printf("recv:%d %s\n",ret,buf);
                    //set_event_send();
                    #if ENABLE_KVSTORE
                    char response[1024] = {0};
                    int send_t = kvs_protocol(buf,ret,response);
                    set_event_send(&ring,resault.fd,response,send_t,0);
                    #endif
                }else{
                    //printf("recv:%d client %d have closed with err\n",ret,resault.fd);
                    close(resault.fd);
                }
                
                //printf("IO_URING_RECV\n");
            }
            else if (resault.event == IO_URING_SEND)
            {
                int ret = entries->res;
                //printf("send:%d message have send\n",ret);
                set_event_recv(&ring,resault.fd,buf,sizeof(buf),0);
            }
            
        }
        io_uring_cq_advance(&ring,nread);
    }

    return 0;
}