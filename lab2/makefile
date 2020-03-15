compile:
	gcc -Wall -g -c program.c
	gcc -o program program.o

generate:
	make compile
	./program generate dane 10 10

sort:
	make compile
	./program sort dane 10 10 lib

sort_test_lib:
	make compile
	./program copy dane_buffor dane1 7 5 lib
	./program sort dane1 7 5 lib

sort_test_sys:
	make compile
	./program copy dane_buffor dane1 7 5 lib
	./program sort dane1 7 5 sys


copy_lib:
	make compile
	./program copy dane_buffor dane1 7 5 lib

copy_sys:
	make compile
	./program copy dane_buffor dane1 7 5 sys