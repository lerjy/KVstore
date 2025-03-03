#ifndef __HEAD_
#define __HEAD_

#include<sys/epoll.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<string.h>


#define __CNN_COUNT_ 1024

#define ENABLE_ECHO     0
#define KVS_MAX_TOKENS 128

#define ENABLE_IO_URING         1
#define ENABLE_REACTOR          1
#define ENABLE_KVSTORE          1
#define KVS_ENABLE_ARRAY        1
#define ENABLE_KVS_RBTREE       1
#define ENABLE_KVS_HASH         1
#define ENABLE_KVS_SKIPTABLE    1


#if KVS_ENABLE_ARRAY

#include"kvs_array.h"

#endif

typedef int(*callback)(int fd);
struct event{
    int fd;
    char read_buf[BUFSIZ];
    int read_len;
    char write_buf[BUFSIZ];
    int write_len;
    int stat;
    union
    {
        callback listen_cb;
        callback read_cb;
    }re_ction;
    callback write_cb;
};

#if WEB_SERVER
int web_quest(struct event *c);
int web_reponse(struct event *c);
#endif


#if ENABLE_KVS_RBTREE

#define RED				1
#define BLACK 			2

#define ENABLE_KEY_CHAR		1

#if ENABLE_KEY_CHAR
typedef char* KEY_TYPE;
#else
typedef int KEY_TYPE; // key
#endif

typedef struct _rbtree_node {
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;
	KEY_TYPE key;
	void *value;
} rbtree_node;

typedef struct _rbtree {
	rbtree_node *root;
	rbtree_node *nil;
} rbtree;


typedef struct _rbtree kvs_rbtree_t;

int kvs_rbtree_create(kvs_rbtree_t *inst);
void kvs_rbtree_destory(kvs_rbtree_t *inst);
int kvs_rbtree_set(kvs_rbtree_t *inst, char *key, char *value);
char* kvs_rbtree_get(kvs_rbtree_t *inst, char *key);
int kvs_rbtree_del(kvs_rbtree_t *inst, char *key);
int kvs_rbtree_mod(kvs_rbtree_t *inst, char *key, char *value);
int kvs_rbtree_exist(kvs_rbtree_t *inst, char *key);



#endif

#if ENABLE_KVS_HASH

#define MAX_KEY_LEN	128
#define MAX_VALUE_LEN	512
#define MAX_TABLE_SIZE	1024

#define ENABLE_KEY_POINTER	1


typedef struct hashnode_s {
#if ENABLE_KEY_POINTER
	char *key;
	char *value;
#else
	char key[MAX_KEY_LEN];
	char value[MAX_VALUE_LEN];
#endif
	struct hashnode_s *next;
	
} hashnode_t;


typedef struct hashtable_s {

	hashnode_t **nodes; //* change **, 

	int max_slots;
	int count;

} hashtable_t;

typedef struct hashtable_s kvs_hash_t;


int kvs_hash_create(kvs_hash_t *hash);
void kvs_hash_destory(kvs_hash_t *hash);
int kvs_hash_set(hashtable_t *hash, char *key, char *value);
char * kvs_hash_get(kvs_hash_t *hash, char *key);
int kvs_hash_mod(kvs_hash_t *hash, char *key, char *value);
int kvs_hash_del(kvs_hash_t *hash, char *key);
int kvs_hash_exist(kvs_hash_t *hash, char *key);


#endif


#if ENABLE_KVSTORE
// int kvs_request(struct event *c);
// int kvs_response(struct event *c);
int kvs_protocol(char *msg,int length,char *response);

void *kvs_malloc(size_t size);
void kvs_free(void *ptr);

int kvs_reactor_start(unsigned short port);
int kvs_ntyco_start(unsigned short port);
int kvs_io_uring_start(unsigned short port);

int kvs_split_token(char *msg,char *token[]);

int init_kvengine(void);
#endif
#endif