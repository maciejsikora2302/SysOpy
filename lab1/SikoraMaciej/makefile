

static_test:
	echo "Static library " >> raport2.txt
	make static
	make small_test 
	make medium_test
	make large_test

static:
	make clear
	
	gcc -Wall -o lib.o -c lib.c
	
	ar crs liblib.a lib.o

	gcc -Wall main.c -o main -L. -llib
	
	rm -f *.o

dynamic:
	make clear

	gcc -Wall -c lib.c

	gcc -Wall -shared -fPIC -o liblib.so lib.o

	gcc main.c -Wall -o main -L. -llib -Wl,-rpath=`pwd`

	rm -f *.o

clear:
	rm -f *.o *.a *.so main


small_test:
	echo "Small Test"
	echo "Small Test (4 files - small files) a.txt b.txt lorem_small.txt lorem_small2.txt"
	echo "Small Test (4 files - small files) a.txt b.txt lorem_small.txt lorem_small2.txt" >> raport2.txt
	./main "compare_pairs" "4" "a.txt:b.txt lorem_small.txt:lorem_small2.txt" "remove_operation" "1" "0" "remove_block" "0"

medium_test:
	echo "Medium Test"
	echo "Medium Test (8 files - medium-sized) a.txt:b.txt lorem_medium.txt:lorem_medium2.txt a.txt:lorem_medium2.txt b.txt:lorem_medium.txt"
	echo "Medium Test (8 files - medium-sized) a.txt:b.txt lorem_medium.txt:lorem_medium2.txt a.txt:lorem_medium2.txt b.txt:lorem_medium.txt" >> raport2.txt
	./main "create_table" "8" "compare_pairs" "8" "a.txt:b.txt lorem_medium.txt:lorem_medium2.txt a.txt:lorem_medium2.txt b.txt:lorem_medium.txt" "remove_block" "0" "remove_operation" "1" "0"


large_test:
	echo "Large Test"
	echo "Large Test (10 files - large/medium) " 
	echo "Large Test (10 files - large/medium) " >> raport2.txt
	./main "create_table" "10" "compare_pairs" "10" "lorem_large.txt:lorem_large2.txt lorem_large.txt:lorem_medium.txt lorem_large.txt:lorem_medium2.txt lorem_large2.txt:lorem_small2.txt lorem_small.txt:lorem_large.txt" "remove_operation" "1" "0" 

