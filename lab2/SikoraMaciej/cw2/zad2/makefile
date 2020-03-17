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
	./find -mtime -2 -atime -10 -maxdepth 3 '/home/fedora_test/Documents/SysOpy/SysOpy/lab2/SikoraMaciej/cz2/zad2/test_dir' sys
	./find -mtime -2 -atime -10 -maxdepth 3 './test_dir' nftw

clean:
	rm -f *.o
	rm -f find