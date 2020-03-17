all:
	clear
	make clean
	make compile
	make test
	
compile:
	gcc -Wall -g -c program.c
	gcc -o program program.o

test:
	clear
	gcc -Wall -g -c program.c
	gcc -o program program.o
	gcc -Wall -g -c run_tests.c
	gcc -o run_tests run_tests.o
	./run_tests

clean:
	rm -f *.o
	rm -f a.out
	rm -f program