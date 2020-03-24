all:
	clear
	make clean
	make compile
	rm -f wynik.txt
	touch wynik.txt
	make test
	make clean
	

compile:
	@gcc -Wall -g -c macierz.c
	@gcc -o macierz macierz.o

aaa:
	gcc -Wall -g -c aaa.c
	gcc -o aaa aaa.o
	./aaa

test:
	./macierz './lista' 2 10 shared
	# ./macierz './lista' 2 2 scattered
	#lista ilość_procesów_potomnych max_czas_życia_w_sekundach wariant_zapisu_wspolny_plik_albo_wiele_malych

testerka:
	clear
	make clean
	rm -f matrix*
	gcc -Wall -g -c testerka.c
	gcc -o testerka testerka.o
	./testerka 1 5 7
	#ilość_macierzy min max

clean:
	rm -f *.o
	rm -f macierz
	rm -f test
	rm -f testerka

try:
	gcc -Wall -g -c test_forks.c
	gcc -o test_forks test_forks.o
	./test_forks
	