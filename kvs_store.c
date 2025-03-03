#include "kvs_server.h"
/*
@return : 
-1  :   The parameter is incorrect
0   :   Memory error
-2  :   
*/

#if ENABLE_KVSTORE

#if KVS_ENABLE_ARRAY
kvs_array_t global_array = {0};
#endif

#if ENABLE_KVS_RBTREE
extern kvs_rbtree_t global_rbtree;
#endif


#if ENABLE_KVS_HASH
extern kvs_hash_t global_hash;
#endif


const char *command[] = {
    "SET","GET","DEL","MOD","EXIST",
    "RSET","RGET","RDEL","RMOD","REXIST",
    "HSET","HGET","HDEL","HMOD","HEXIST"
};

enum {
    //array
    KVS_CMD_START = 0,
    KVS_CMD_SET = KVS_CMD_START,
    KVS_CMD_GET,
    KVS_CMD_DEL,
    KVS_CMD_MOD,
    KVS_CMD_EXIST,

    //rbtree
    KVS_CMD_RSET,
    KVS_CMD_RGET,
    KVS_CMD_RDEL,
    KVS_CMD_RMOD,
    KVS_CMD_REXIST,

    //hash
    KVS_CMD_HSET,
    KVS_CMD_HGET,
    KVS_CMD_HDEL,
    KVS_CMD_HMOD,
    KVS_CMD_HEXIST,


    KVS_CMD_COUNT
};

const char *response[] = {

};

void *kvs_malloc(size_t size){
    return malloc(size);
}
void kvs_free(void *ptr){
    return free(ptr);
}

int kvs_split_token(char *msg,char *tokens[]){
    if(msg == NULL || tokens == NULL) return -1;
    char *token = strtok(msg," ");
    int index = 0;
    while(token != NULL){
        //printf("index %d : %s\n",index,token);
        tokens[index] = token;
        token = strtok(NULL," ");
        index++;
    }
    return index;
}

/*
SET KEY VALUE
tonkens[0] : SET
tonkens[1] : KEY
tonkens[3] : VALUE
*/

int kvs_protocol_filter(char **tokens,int count,char *response){
    if(tokens[0] == NULL || count == 0 || response == NULL) return -1;
    
    int cmd = KVS_CMD_START;
    for(cmd = KVS_CMD_START; cmd < KVS_CMD_COUNT;cmd ++){
        //printf("tokens[0] = %s, command[%d] = %s\n",tokens[0],cmd,command[cmd]);
        if(strcmp(tokens[0],command[cmd]) == 0){
            break;
        }
    }
    int ret = 0;
    int length = 0;
    //printf("cmd = %d \n",cmd);

    switch(cmd){
        //array

#if KVS_ENABLE_ARRAY
    case KVS_CMD_SET:
        ret = kvs_array_set(&global_array,tokens[1],tokens[2]);
        if(ret == 0){
            length = sprintf(response, "SUCCESS\r\n");
            break;
        }else if(ret < 0){
            length = sprintf(response, "ERROR\r\n");
            break;
        }else{
            length = sprintf(response, "EXIST\r\n");
            break;
        }
    case KVS_CMD_GET:
        char *value = kvs_array_get(&global_array,tokens[1]);
        if(value == NULL){
            length = sprintf(response, "NO EXIST\r\n");
            break;
        }else{
            length = sprintf(response, "%s\r\n",value);
            break;
        }
    case KVS_CMD_DEL:
        ret = kvs_array_del(&global_array, tokens[1]);
        if (ret == 0){
            length = sprintf(response, "SUCCESS\r\n");
            break;
        }else if (ret < 0){
            length = sprintf(response, "ERROR\r\n");
            break;
        }else{
            length = sprintf(response, "NO EXIST\r\n");
            break;
        }
    case KVS_CMD_MOD:
        ret = kvs_array_mod(&global_array,tokens[1],tokens[2]);
        if(ret == 0){
            length = sprintf(response, "SUCCESS\r\n");
            break;
        }else if(ret < 0){
            length = sprintf(response, "ERROR\r\n");
            break;
        }else{
            length = sprintf(response, "NO EXIST\r\n");
            break;
        }
    case KVS_CMD_EXIST:
        ret = kvs_array_exist(&global_array,tokens[1]);
        if(ret == 0){
            length = sprintf(response, "EXIST\r\n");
            break;
        }else if(ret < 0){
            length = sprintf(response, "ERROR\r\n");
            break;
        }else{
            length = sprintf(response, "NO EXIST\r\n");
            break;
        }
#endif


#if ENABLE_KVS_RBTREE
    //rbtree
    case KVS_CMD_RSET:
        ret = kvs_rbtree_set(&global_rbtree,tokens[1],tokens[2]);
        if(ret == 0){
            length = sprintf(response, "SUCCESS\r\n");
            break;
        }else if(ret < 0){
            length = sprintf(response, "ERROR\r\n");
            break;
        }else{
            length = sprintf(response, "EXIST\r\n");
            break;
        }
    case KVS_CMD_RGET:
        value = kvs_rbtree_get(&global_rbtree,tokens[1]);
        if(value == NULL){
            length = sprintf(response, "NO EXIST\r\n");
            break;
        }else{
            length = sprintf(response, "%s\r\n",value);
            break;
        }
    case KVS_CMD_RDEL:
        ret = kvs_rbtree_del(&global_rbtree, tokens[1]);
        if (ret == 0){
            length = sprintf(response, "SUCCESS\r\n");
            break;
        }else if (ret < 0){
            length = sprintf(response, "ERROR\r\n");
            break;
        }else{
            length = sprintf(response, "NO EXIST\r\n");
            break;
        }
    case KVS_CMD_RMOD:
        ret = kvs_rbtree_mod(&global_rbtree,tokens[1],tokens[2]);
        if(ret == 0){
            length = sprintf(response, "SUCCESS\r\n");
            break;
        }else if(ret < 0){
            length = sprintf(response, "ERROR\r\n");
            break;
        }else{
            length = sprintf(response, "NO EXIST\r\n");
            break;
        }
    case KVS_CMD_REXIST:
        ret = kvs_rbtree_exist(&global_rbtree,tokens[1]);
        if(ret == 0){
            length = sprintf(response, "EXIST\r\n");
            break;
        }else if(ret < 0){
            length = sprintf(response, "ERROR\r\n");
            break;
        }else{
            length = sprintf(response, "NO EXIST\r\n");
            break;
        }
#endif


#if ENABLE_KVS_HASH
	case KVS_CMD_HSET:
		ret = kvs_hash_set(&global_hash ,tokens[1], tokens[2]);
		if (ret < 0) {
			length = sprintf(response, "ERROR\r\n");
		} else if (ret == 0) {
			length = sprintf(response, "SUCCESS\r\n");
		} else {
			length = sprintf(response, "EXIST\r\n");
		} 
		
		break;
	case KVS_CMD_HGET: {
		char *result = kvs_hash_get(&global_hash, tokens[1]);
		if (result == NULL) {
			length = sprintf(response, "NO EXIST\r\n");
		} else {
			length = sprintf(response, "%s\r\n", result);
		}
		break;
	}
	case KVS_CMD_HDEL:
		ret = kvs_hash_del(&global_hash,tokens[1]);
		if (ret < 0) {
			length = sprintf(response, "ERROR\r\n");
 		} else if (ret == 0) {
			length = sprintf(response, "SUCCESS\r\n");
		} else {
			length = sprintf(response, "NO EXIST\r\n");
		}
		break;
	case KVS_CMD_HMOD:
		ret = kvs_hash_mod(&global_hash ,tokens[1], tokens[2]);
		if (ret < 0) {
			length = sprintf(response, "ERROR\r\n");
 		} else if (ret == 0) {
			length = sprintf(response, "SUCCESS\r\n");
		} else {
			length = sprintf(response, "NO EXIST\r\n");
		}
		break;
	case KVS_CMD_HEXIST:
		ret = kvs_hash_exist(&global_hash ,tokens[1]);
		if (ret == 0) {
			length = sprintf(response, "EXIST\r\n");
		} else {
			length = sprintf(response, "NO EXIST\r\n");
		}
		break;
#endif


    default:
        length = sprintf(response, "ERROR\r\n");
        break;
    }
    //memset(msg,0,sizeof(msg));
    //memset(response,0,sizeof(response));
    return length;
}

/*
msg:request message
length:request massage length
response:msg of response
@return:length of response
*/
int kvs_protocol(char *msg,int length,char *response){
    if(msg == NULL || length <= 0 || response == NULL) return -1;
    //printf("recv %d words : %s \n", length,msg);
    char *tokens[KVS_MAX_TOKENS] = {0};
    int count = kvs_split_token(msg,tokens);

    if(count == 0) return -1;
    int ret = kvs_protocol_filter(tokens,count,response);
    //printf("ret = %d , response : %s \n",ret,response);
    //memcpy(response,msg,length);
    return ret;
}


int init_kvengine(void){

#if KVS_ENABLE_ARRAY
	kvs_array_create(&global_array);
#endif

#if ENABLE_KVS_RBTREE
	kvs_rbtree_create(&global_rbtree);
#endif

#if ENABLE_KVS_HASH
	kvs_hash_create(&global_hash);
#endif

	return 0;
}


void dest_kvengine(void) {
#if KVS_ENABLE_ARRAY
	kvs_array_destory(&global_array);
#endif
#if ENABLE_KVS_RBTREE
	kvs_rbtree_destory(&global_rbtree);
#endif
#if ENABLE_KVS_HASH
	kvs_hash_destory(&global_hash);
#endif

}

// int kvs_request(struct event *c){
//     printf("recv %d words : %s \n", c->read_len,c->read_buf);
// }
// int kvs_response(struct event *c){
//     printf("do nothing\n");
// }


int main(int argc,char *argv[]){
    if(argc != 3) return 0;
    unsigned short port = atoi(argv[1]);
    if(init_kvengine() == -1) return 0;

    // 1-reactor    2-ntyco     3-io_uring
    if(atoi(argv[2]) == 1){
        kvs_reactor_start(port);
    }else if(atoi(argv[2]) == 2){
        kvs_ntyco_start(port);
    }else if(atoi(argv[2]) == 3){
        kvs_io_uring_start(port);
    }
    dest_kvengine();
    return 0;
}
#endif