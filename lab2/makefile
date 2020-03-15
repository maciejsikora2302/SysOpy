compile:
	gcc -Wall -g -c program.c
	gcc -o program program.o

generate:
	make compile
	./program generate dane 10 10

sort:
	make compile
	./program sort dane 10 10 lib

sort_test:
	make compile
	./program copy dane_buffor dane1 7 5 lib
	./program sort dane1 7 5 lib

copy_lib:
	make compile
	./program copy dane_buffor dane1 7 5 lib

copy_sys:
	make compile
	./program copy dane_buffor dane1 7 5 sys