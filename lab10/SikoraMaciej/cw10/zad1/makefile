all:
	make clean
	clear
	@make compile -s
	
compile:
	@gcc -Wall -g -c common.c

	@gcc -Wall -g -c client.c
	@gcc -o client client.o common.o -lpthread -lm

	@gcc -Wall -g -c server.c
	@gcc -o server server.o common.o -lpthread -lm

server:
	./server 9002 .unix_socket

client:
	./client my_name1 net 127.0.0.1 9002

clean:
	@rm -f *.o
	@rm -f client server
	@echo "Clean"