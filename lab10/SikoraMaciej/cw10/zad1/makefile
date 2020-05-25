all:
	make clean
	clear
	@make compile -s
	
compile:
	@gcc -Wall -g -c client.c
	@gcc -o client client.o

	@gcc -Wall -g -c server.c
	@gcc -o server server.o

server:
	./server 9002 ./unix_socket

client:
	./client my_name1 net 9002

clean:
	@rm -f *.o
	@rm -f client server