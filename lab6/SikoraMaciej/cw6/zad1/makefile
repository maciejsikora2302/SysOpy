all:
	make clean
	clear
	@make compile -s
	@make run -s
	@make clean -s

compile:
	@gcc -Wall -g -c server.c
	@gcc -o server server.o

run:
	./server

clean:
	@rm -f *.o
	@rm -f server client
