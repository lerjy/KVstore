#include"wrap.h"
#include"kvs_server.h"

#define __EVENT_IN_ EPOLLIN
#define __EVENT_OUT_ EPOLLOUT

#if 0
#define __CNN_COUNT_ 1024
typedef int(*callback)(int fd);
int flag;
struct event{
    int fd;
    char read_buf[BUFSIZ];
    int read_len;
    char write_buf[BUFSIZ];
    int write_len;
    union
    {
        callback listen_cb;
        callback read_cb;
    }re_ction;
    callback write_cb;
};
#endif
struct event cnn_list[__CNN_COUNT_] = {0};
void set_event(int fd, int event, int flag);
int epfd = 0;
int read_cb(int cfd)
{
    memset(cnn_list[cfd].read_buf,0,BUFSIZ);
    //char *msg = malloc(sizeof(cnn_list[cfd].read_buf));
    //memset(msg,0,sizeof(cnn_list[cfd].read_buf));
    int count = recv(cfd, cnn_list[cfd].read_buf, BUFSIZ, 0);
    if (count == 0)
    {
        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
        close(cfd);
        //printf("client disconnect\n");
        return 0;
    }
    cnn_list[cfd].read_len = count;
#if WEB_SERVER
    web_quest(&cnn_list[cfd]);
#endif
#if ENABLE_ECHO //ehco
    printf("recv %d words : %s \n", cnn_list[cfd].read_len,cnn_list[cfd].read_buf);
    cnn_list[cfd].write_len = cnn_list[cfd].read_len;
    memcpy(cnn_list[cfd].write_buf, cnn_list[cfd].read_buf, cnn_list[cfd].write_len);
    cnn_list[cfd].stat = 0;
#endif 

#if ENABLE_KVSTORE
    //kvs_request(&cnn_list[cfd]);
    cnn_list[cfd].write_len = kvs_protocol(cnn_list[cfd].read_buf,cnn_list[cfd].read_len,cnn_list[cfd].write_buf);
#endif
    //free(msg);
    set_event(cfd, __EVENT_OUT_, 0);
    return count;
}
int write_cb(int cfd)
{
    //kvs_response(&cnn_list[cfd]);
    int count = send(cfd, cnn_list[cfd].write_buf, cnn_list[cfd].write_len, 0);
#if ENABLE_ECHO
    int count = send(cfd, cnn_list[cfd].write_buf, cnn_list[cfd].write_len, 0);
    printf("send %d words : %s\n", cnn_list[cfd].write_len,cnn_list[cfd].write_buf);
#endif
#if WEB_SERVER
    if(cnn_list[cfd].stat == 0){
        web_reponse(&cnn_list[cfd]);
        int count = send(cfd, cnn_list[cfd].write_buf, cnn_list[cfd].write_len, 0);
        printf("send %d words \n", cnn_list[cfd].write_len);
    }else if(cnn_list[cfd].stat == 1){    
        #if WEB_SERVER
        web_reponse(&cnn_list[cfd]);
        #endif
    }else{
        set_event(cfd,__EVENT_IN_,0);
    }
#endif
    set_event(cfd,__EVENT_IN_,0);
    return cnn_list[cfd].write_len;
}
int listen_cb(int lfd){
    struct sockaddr_in c_addr;
    socklen_t c_addr_len = sizeof(c_addr);
    int clientfd = Accept(lfd,(struct sockaddr*)&c_addr,&c_addr_len);
    cnn_list[clientfd].fd = clientfd;
    cnn_list[clientfd].stat = 0;
    cnn_list[clientfd].re_ction.read_cb = read_cb;
    cnn_list[clientfd].write_cb = write_cb;
    set_event(clientfd,__EVENT_IN_,1);
    //printf("client connected\n");
    return clientfd;
}
static int init_sock(unsigned short port){
    struct sockaddr_in s_addr;
    bzero(&s_addr,sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0代表机器任意的IP地址
    int socketfd = Socket(AF_INET,SOCK_STREAM,0);
    // 端口被绑定后，不能立即再次使用，setsockopt设置OPT状态设置端口复用
    /*int setsockopt(int sockfd, int level, int optname,
                    const void optval[.optlen],
                    socklen_t optlen);*/
    int opt = 1;
    Setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    Bind(socketfd, (struct sockaddr *)&s_addr, sizeof(s_addr));
    Listen(socketfd,128);
    return socketfd;
}
void set_event(int fd,int event,int flag){
    if(flag){
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = event;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    }
    else{
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = event;
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
    }
}
int kvs_reactor_start(unsigned short port){
    //if(args != 2) return 0;
    int nready = 0;
    //unsigned short port = atoi(argv[1]);
    int sockfd = init_sock(port);
    epfd = epoll_create(1024);

    cnn_list[sockfd].fd = sockfd;
    cnn_list[sockfd].re_ction.listen_cb = listen_cb;

    set_event(sockfd,__EVENT_IN_,1);

    struct epoll_event events[1024] = {0};
    while(1){
        nready = epoll_wait(epfd,events,1024,-1);
        for(int i = 0;i<nready;i++){
            int fd = events[i].data.fd;
            if(events[i].events & EPOLLIN){
                cnn_list[fd].re_ction.read_cb(fd);
            }
            if(events[i].events & EPOLLOUT){
                cnn_list[fd].write_cb(fd);
            }
        }
    }
    return 0;
}
