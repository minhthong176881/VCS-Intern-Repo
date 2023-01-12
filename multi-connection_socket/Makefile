.PHONY: client multiprocess_server

client:
	mkdir -p build
	gcc sources/client.c sources/inet_sockets.c -o build/client

multiprocess_server:
	mkdir -p build
	gcc sources/multiprocess_server.c sources/inet_sockets.c -o build/multiprocess_server

multithread_server:
	mkdir -p build
	gcc sources/multithread_server.c sources/inet_sockets.c -o build/multithread_server -lpthread