all:
	make static
	make shared

static:
	make clear
	
	gcc -g -Wall -o lib.o -c lib.c
	
	ar rcs liblib.a lib.o

	gcc -g -Wall main.c -o main -L. -llib

	./main create_table 4 compare_pairs "a.txt:b.txt a.txt:b.txt" remove_block 0 remove_operation 1 1
	
	make clear

shared:
	make clear

	gcc -Wall -fPIC -c lib.c -o lib.o

	gcc -shared -o liblib.so lib.o

	gcc -L. -Wl,-rpath=. -Wall -o main main.c -llib 

	./main create_table 4 compare_pairs "a.txt:b.txt a.txt:b.txt" remove_block 0 remove_operation 1 1
	
	rm -f *.o

	make clear

clear:
	rm -f *.o *.a *.so main tmp*