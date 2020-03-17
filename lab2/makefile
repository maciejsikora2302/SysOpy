compile_find:
	clear
	gcc -Wall -g -c find.c
	gcc -o find find.o
	# ./find './test_dir'
	clear
	./find -mtime -2 -atime -10 -maxdepth 3 '/home/fedora_test/Documents/SysOpy/SysOpy/lab2/test_dir' sys
	./find -mtime -2 -atime -10 -maxdepth 3 '/home/fedora_test/Documents/SysOpy/SysOpy/lab2/test_dir' nftw


run_test_program:
	gcc -Wall -g -c program.c
	gcc -o program program.o
	gcc -Wall -g -c run_tests.c
	gcc -o run_tests run_tests.o
	./run_tests

compile:
	gcc -Wall -g -c program.c
	gcc -o program program.o

cp_test:
	gcc -Wall -g -c run_tests.c
	gcc -o run_tests run_tests.o
	./run_tests
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

time_tests:
	make compile
	./program generate generated_data 100 1
	./program copy generated_data data_to_sort_sys 100 1



	./program generate generated_data 100 4
	./program generate generated_data 100 512
	./program generate generated_data 100 1024
	./program generate generated_data 100 4096
	./program generate generated_data 100 8192

	./program generate generated_data 200 1
	./program generate generated_data 200 4
	./program generate generated_data 200 512
	./program generate generated_data 200 1024
	./program generate generated_data 200 4096
	./program generate generated_data 200 8192



sorting_sys_time_test:
	./program 