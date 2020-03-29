all:
	clear
	@make -s clean
	@make -s compile
	@make -s compile_child
	# make --no-print-directory test
	# @make -s clean
	

compile:
	@gcc -Wall -g -c prog.c
	@gcc -o prog prog.o

compile_child:
	@gcc -Wall -g -c child.c
	@gcc -o child child.o

test:
	./prog

clean:
	rm -f *.o
	rm -f prog
	rm -f child
