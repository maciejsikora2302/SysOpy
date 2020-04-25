all:
	make clean
	clear
	@make compile -s
	
compile:
	@gcc -Wall -g -c server.c
	@gcc -Wall -g -c common_utils.c
	@gcc -o server server.o common_utils.o

run:
	./server

clean:
	@rm -f *.o
	@rm -f server client
