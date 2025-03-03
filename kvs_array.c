#include "kvs_array.h"




#if KVS_ENABLE_ARRAY

int kvs_array_create(kvs_array_t *inst){
    if(!inst) return -1;
    if(inst->table != NULL){
        printf("table has exist\n");
        return -1;
    }
    memset(inst, 0, sizeof(kvs_array_t));
    inst->table = kvs_malloc(KVS_ARRAY_SIZE * sizeof(kvs_array_item_t));
    if(!inst->table){
        return -1;
    }
    inst->total = 0;
    return 0;
}
void kvs_array_destory(kvs_array_t *inst){
    if(!inst) return;
    if(inst->table){
        return kvs_free(inst);
    }
}

/*
@return : <0:error  =0:success  >1:exist-faild
*/

int kvs_array_set(kvs_array_t *inst,char *key,char *value){
    if(inst == NULL || key == NULL || value == NULL) return -1;
    if(inst->index == KVS_ARRAY_SIZE) return -1;

    if(kvs_array_get(inst,key) != NULL){
        return 1;
    }
    //printf("memmory\n");
    char *kkey = kvs_malloc(strlen(key) + 1);
    if(!kkey) return -2;
    memset(kkey,0,sizeof(key)+1);
    strncpy(kkey,key,strlen(key));

    char *kvalue = kvs_malloc(strlen(value)+1);
    if(!kvalue) return -2;
    memset(kvalue,0,sizeof(value)+1);
    strncpy(kvalue,value,strlen(value));

    //printf("for\n");
    int i = 0;
    for(i = 0;i < inst->total;i++){
        if(inst->table[i].key == NULL){
            inst->table[i].key = kkey;
            inst->table[i].value = kvalue;
            //inst->total ++;
            return 0;
        }
    }
    if(i == inst->total && i < KVS_ARRAY_SIZE){
            inst->table[i].key = kkey;
            inst->table[i].value = kvalue;
            inst->total ++;
    }
    return 0;
}

/*
@return : NULL:error or faild   !NULL:success
*/
char * kvs_array_get(kvs_array_t *inst,char *key){
    if(inst == NULL || key == NULL) return NULL;
    //printf("key : %s\n",key);
    int i = 0;
    
    for(i = 0 ; i < inst->total;i++){
        if(inst->table[i].key == NULL) continue;
        if(strcmp(inst->table[i].key,key) == 0){
            //printf("%s\n",inst->table[i].value);
            return inst->table[i].value;
        }
    }
    return NULL;
}

/*
@return : <0:error  =0:success  >0:no exist - faild
*/

int kvs_array_del(kvs_array_t *inst,char *key){
    if(inst == NULL || key == NULL) return -1;
    int i = 0;
    for(i = 0 ; i < inst->total;i++){
        if(inst->table[i].key == NULL) continue;
        if(strcmp(inst->table[i].key,key) == 0){
            //memset(inst->table[i].key,0,sizeof(inst->table[i].key));
            kvs_free(inst->table[i].key);
            inst->table[i].key == NULL;

            //memset(inst->table[i].value,0,sizeof(inst->table[i].value));
            kvs_free(inst->table[i].value);
            inst->table[i].value = NULL;

            if (inst->total-1 == i) {
			inst->total --;
			}

            return 0;
        }
    }
    return 1;
}

/*
@return :   <0:error    0:success   1:no exist - faild
*/

int kvs_array_mod(kvs_array_t *inst,char *key,char *value){
    if(inst == NULL || key == NULL || value == NULL) return -1;
    int i = 0;
    for(i = 0 ; i < inst->total;i++){
        if(inst->table[i].key == NULL) continue;
        if(strcmp(inst->table[i].key,key) == 0){
            //memset(inst->table[i].value,0,sizeof(inst->table[i].value));
            kvs_free(inst->table[i].value);
            char *kvalue = kvs_malloc(strlen(value)+1);
            if(!kvalue) return -2;
            memset(kvalue,0,sizeof(value)+1);
            strncpy(kvalue,value,strlen(value));
            inst->table[i].value = kvalue;
            return 0;
        }
    }
    return 1;
}

/*
@return :   -1:error    0:success   1:no exist - faild
*/

int kvs_array_exist(kvs_array_t *inst,char *key){
    if(inst == NULL || key == NULL) return -1;
    // char *value = {0};
    // int ret = sprintf(value,"%s",kvs_array_get(inst,key));
    // printf("%s\n",value);
    if(kvs_array_get(inst,key) != NULL){
        return 0;
    }
    return 1;
}


#endif