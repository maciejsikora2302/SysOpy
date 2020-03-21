all:
	clear
	make clean
	make compile
	make test
	make clean
	

compile:
	gcc -Wall -g -c find.c
	gcc -o find find.o

test:
	./find './test_dir' sys
	# ./find -mtime -2 -atime -10 -maxdepth 3 './test_dir' nftw

clean:
	rm -f *.o
	rm -f find