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


small_test:
	echo "Small Test"
	echo "Small Test (4 files - small files) a.txt b.txt lorem_small.txt lorem_small2.txt"
	echo "Small Test (4 files - small files) a.txt b.txt lorem_small.txt lorem_small2.txt" >> raport2.txt
	./main compare_pairs 4 "a.txt:b.txt lorem_small.txt:lorem_small2.txt" remove_operation 1 0 remove_block 0

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

