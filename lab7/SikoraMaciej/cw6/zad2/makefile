all:
	make clean
	clear
	@make compile -s

compile:
	@gcc -Wall -g -c server.c
	@gcc -o server server.o
	@gcc -Wall -g -c client.c
	@gcc -o client client.o

run:
	./server &

clean:
	@rm -f *.o
	@rm -f server client
