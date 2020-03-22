all:
	clear
	make clean
	make compile
	rm -f wynik.txt
	touch wynik.txt
	make test
	make clean
	

compile:
	gcc -Wall -g -c macierz.c
	gcc -o macierz macierz.o

aaa:
	gcc -Wall -g -c aaa.c
	gcc -o aaa aaa.o
	./aaa

test:
	./macierz './lista' 10 10 shared
	# ./macierz './lista' 2 10 scattered
	# scattered
	#lista ilość_procesów_potomnych max_czas_życia_w_sekundach wariant_zapisu_wspolny_plik_albo_wiele_malych

clean:
	rm -f *.o
	# rm -f macierz
	rm -f test

try:
	gcc -Wall -g -c test_forks.c
	gcc -o test_forks test_forks.o
	./test_forks
	