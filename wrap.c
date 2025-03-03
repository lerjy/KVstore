#include"wrap.h"

void sys_err(char *er){
	perror(er);
	exit(1);
}
void systhread_err(int num,char *er){
	fprintf(stderr,er,strerror(num));
	exit(1);
}
int Socket(int domain, int type, int protocol){
    int sockfd = socket(domain,type,protocol);
    if(sockfd == -1){
        sys_err("socket error:");
    }
    return sockfd;
}

int Setsockopt(int sockfd, int level, int optname,const void *optval,socklen_t optlen){
    int ret = setsockopt(sockfd,level,optname,optval,optlen);
    if(ret == -1){
        sys_err("setsockopt error:");
    }
}
int Bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen){
	int ret = 0;
	ret = bind(sockfd,addr,addrlen);
	if(ret == -1)
		sys_err("bind error");
	return ret;
}
int Listen(int sockfd,int backlog){
	int ret = 0;
	ret = listen(sockfd,backlog);
	if(ret == -1)
		sys_err("listen error");
	return ret;
}
int Accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen){
	int ret = 0;
	ret = accept(sockfd,addr,addrlen);
	if(ret == -1)
		sys_err("accept error");
	return ret;
}
int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen){
	int ret = connect(sockfd,addr,addrlen);
	if(ret == -1){
		sys_err("connect error");
		exit(1);
	}
	return ret;
}
int Pthread_create(pthread_t *thread,const pthread_attr_t *attr,void *(*start_routine)(void *),void *arg){
	int ret = 0;
	ret = pthread_create(thread,attr,start_routine,arg);
	if(ret != 0)
		systhread_err(ret,"pthread_create error");
	return ret;
}
