all:
	make clean
	clear
	@make compile -s
	@make run -s
	@make clean -s

compile:
	@gcc -Wall -g -c prog.c
	@gcc -o prog prog.o

run:
	./prog

clean:
	@rm -f *.o
	@rm -f prog