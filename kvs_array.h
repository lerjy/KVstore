#ifndef __KVS_ARRAY_HEAD__
#define __KVS_ARRAY_HEAD__

#include "kvs_server.h"

#if KVS_ENABLE_ARRAY

#define KVS_ARRAY_SIZE 1024

typedef struct kvs_array_item_s{
    char *key;
    char *value;
}kvs_array_item_t;

typedef struct kvs_array_s{
    kvs_array_item_t *table;
    int index;
    int total;
}kvs_array_t;



int kvs_array_create(kvs_array_t *inst);
void kvs_array_destory(kvs_array_t *inst);

int kvs_array_set(kvs_array_t *inst,char *key,char *value);
char * kvs_array_get(kvs_array_t *inst,char *key);
int kvs_array_del(kvs_array_t *inst,char *key);
int kvs_array_mod(kvs_array_t *inst,char *key,char *value);
int kvs_array_exist(kvs_array_t *inst,char *key);
#endif
#endif