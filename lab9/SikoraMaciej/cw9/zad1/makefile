all:
	make clean
	clear
	@make compile -s
	@make run
	
compile:
	@gcc -Wall -g -c main.c
	@gcc -o main main.o -lpthread -lm

run:
	./main 5 10

clean:
	@rm -f *.o
	@rm -f main testerka 