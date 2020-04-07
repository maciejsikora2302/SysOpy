all:
	make clean
	clear
	@make compile -s
	@cp backup.txt to_sort.txt
	@make run -s
	@make clean -s

compile:
	@gcc -Wall -g -c prog.c
	@gcc -o prog prog.o

run:
	./prog to_sort.txt

clean:
	@rm -f *.o
	@rm -f prog