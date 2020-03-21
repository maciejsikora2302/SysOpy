all:
	clear
	make clean
	make compile
	make test
	make clean
	

compile:
	gcc -Wall -g -c macierz.c
	gcc -o macierz macierz.o

test:
	./macierz './lista' 2 10 shared
	./macierz './lista' 2 10 scattered
	# scattered
	#lista ilość_procesów_potomnych max_czas_życia_w_sekundach wariant_zapisu_wspolny_plik_albo_wiele_malych

clean:
	rm -f *.o
	rm -f macierz 2
	