#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include<ctype.h>
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<pthread.h>
#include<error.h>

void sys_err(char *er);
void systhread_err(int num,char *er);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
int Listen(int sockfd,int backlog);
int Accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
int Pthread_create(pthread_t *thread,const pthread_attr_t *attr,void *(*start_routine)(void *),void *arg);
int Setsockopt(int sockfd, int level, int optname,const void *optval,socklen_t optlen);