all:
	clear
	@make -s clean
	@make -s compile
	@chmod 777 ./sender.c
	

compile:
	@gcc -Wall -g -c sender.c
	@gcc -o sender sender.o
	@gcc -Wall -g -c catcher.c
	@gcc -o catcher catcher.o


clean:
	rm -f *.o
	rm -f sender
	rm -f catcher