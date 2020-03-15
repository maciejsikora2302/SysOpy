compile:
	gcc -Wall -g -c program.c
	gcc -o program program.o

generate:
	make compile
	./program generate dane 10 10

sort:
	make compile
	./program sort dane 10 10 lib
