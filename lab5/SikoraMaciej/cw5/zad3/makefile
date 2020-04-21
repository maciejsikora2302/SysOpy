all:
	make clean
	clear
	@make compile -s
	@make run -s
	@make clean -s

compile:
	@gcc -Wall -g -c prog.c
	@gcc -o prog prog.o
	@gcc -Wall -g -c producer.c
	@gcc -o producer producer.o
	@gcc -Wall -g -c consumer.c
	@gcc -o consumer consumer.o

run:
	./prog

clean:
	@rm -f *.o
	@rm -f prog consumer producer pipe
