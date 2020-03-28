all:
	clear
	@make -s clean
	@make -s compile
	# make --no-print-directory test
	# @make -s clean
	

compile:
	@gcc -Wall -g -c prog.c
	@gcc -o prog prog.o

test:
	./prog

clean:
	rm -f *.o
	rm -f prog