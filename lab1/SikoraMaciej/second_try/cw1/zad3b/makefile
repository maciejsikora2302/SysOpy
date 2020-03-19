CC_O = gcc -g -Wall -O
CC_O1 = gcc -g -Wall -O1
CC_O2 = gcc -g -Wall -O2
CC_O3 = gcc -g -Wall -O3
CC_O0 = gcc -g -Wall -O0
CC_Os = gcc -g -Wall -Os

all: static_o static_o0 static_o1 static_o2 static_o3 static_os

static_o:
	make clear
	
	$(CC_O) -o lib.o -c lib.c
	
	ar rcs liblib.a lib.o

	$(CC_O)  main.c -o main -L. -llib

	make test

static_o1:
	make clear
	
	$(CC_O1) -o lib.o -c lib.c
	
	ar rcs liblib.a lib.o

	$(CC_O1)  main.c -o main -L. -llib

	make test

static_o2:

	
	$(CC_O2) -o lib.o -c lib.c
	
	ar rcs liblib.a lib.o

	$(CC_O2)  main.c -o main -L. -llib

	make test
	

static_o3:

	
	$(CC_O3) -o lib.o -c lib.c
	
	ar rcs liblib.a lib.o

	$(CC_O3)  main.c -o main -L. -llib

	make test
	


static_o0:

	
	$(CC_O0) -o lib.o -c lib.c
	
	ar rcs liblib.a lib.o

	$(CC_O0)  main.c -o main -L. -llib

	make test
	


static_os:

	
	$(CC_Os) -o lib.o -c lib.c
	
	ar rcs liblib.a lib.o

	$(CC_Os)  main.c -o main -L. -llib

	make test
	




clear:
	rm -f *.o *.a *.so tmp*

test: small_test medium_test large_test add_and_remove_test

small_test:
	echo "Small Test (4 files)"
	./main create_table 4 compare_pairs "a.txt:b.txt lorem_small.txt:a.txt" remove_operation 1 0 remove_block 0

medium_test:
	echo "Medium Test (8 files)"
	./main create_table 4 compare_pairs "a.txt:b.txt medium_similar.txt:b_medium2.txt"
	./main create_table 4 compare_pairs "b.txt:lorem_small.txt b_medium_no_sim.txt:medium_no_sim.txt"


large_test:
	echo "Large Test (16 files)" 
	./main create_table 16 compare_pairs "a.txt:b.txt a.txt:medium_no_sim.txt large.txt:large2.txt lorem_small.txt:large.txt large2.txt:b.txt lorem_small.txt:large2.txt medium_similar.txt:b_medium_no_sim.txt lorem_small.txt:b.txt"
	

add_and_remove_test:
	echo "Add and remove test"
	./main add_and_remove 1000 2 "a.txt:b.txt"