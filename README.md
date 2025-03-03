# KVstore
A simple KV storage solution that uses multiple network connection modes and multiple data structures


#build This project
gcc -o kvs_server kvs_store.c reactor.c kvs_array.c wrap.c io_uring.c kvs_NtCo_server.c -I NtyCo-master/core/ -L NtyCo-master/ -lntyco -luring
