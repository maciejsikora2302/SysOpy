static:
	make clear
	
	gcc -g -Wall -o lib.o -c lib.c
	
	ar rcs liblib.a lib.o

	gcc -g -Wall main.c -o main -L. -llib
	

shared:
	make clear

	gcc -Wall -fPIC -c lib.c -o lib.o

	gcc -shared -o liblib.so lib.o

	gcc -L. -Wl,-rpath=. -Wall -o main main.c -llib 

dynamic:
	make shared

	gcc -Wall dynamic_main.c -o dynamic_main -ldl -L . -D DYNAMIC

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

dynamic_test:
	./dynamic_main create_table 4 compare_pairs "a.txt:b.txt lorem_small.txt:a.txt"
	./dynamic_main create_table 4 compare_pairs "a.txt:b.txt medium_similar.txt:b_medium2.txt"
	./dynamic_main create_table 4 compare_pairs "b.txt:lorem_small.txt b_medium_no_sim.txt:medium_no_sim.txt"
	./dynamic_main create_table 16 compare_pairs "a.txt:b.txt a.txt:medium_no_sim.txt large.txt:large2.txt lorem_small.txt:large.txt large2.txt:b.txt lorem_small.txt:large2.txt medium_similar.txt:b_medium_no_sim.txt lorem_small.txt:b.txt"