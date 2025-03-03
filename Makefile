CMD = gcc

SRC = kvs_store.c reactor.c wrap.c io_uring.c kvs_NtCo_server.c kvs_array.c kvs_rbtree.c kvs_hash.c

INC = -I NtyCo-master/core/

LIB = -L NtyCo-master/ -lntyco -luring

DIT = kvs_server

ALL:
	$(CMD) -o $(DIT) $(SRC) $(INC) $(LIB)

clean:
	rm -rf $(DIT)
