all:
	clear
	@make -s clean
	@make -s compile
	# make --no-print-directory test
	# @make -s clean
	

compile:
	@gcc -Wall -g -c prog.c
	@gcc -o prog prog.o


clean:
	rm -f *.o
	rm -f prog