all:
	make clean
	clear
	@make compile -s
	
compile:
	@gcc -Wall -g -c main.c
	@gcc -Wall -g -c util.c
	@gcc -Wall -g -c testerka.c
	@gcc -o main main.o util.o -lpthread -lm

run:
	./main 2 interleaved "gator.ascii.pgm" "output.txt"

small_test:
	./main 2 sign "gator.ascii.pgm" "output.txt"
	./main 4 block "gator.ascii.pgm" "output.txt"
	./main 8 interleaved "gator.ascii.pgm" "output.txt"

test:
	@gcc -Wall -g -c testerka.c
	@gcc -o testerka testerka.o
	./testerka

clean:
	@rm -f *.o
	@rm -f main testerka 