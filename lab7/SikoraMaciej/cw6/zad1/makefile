all:
	make clean
	clear
	@make compile -s
	
compile:
	@gcc -Wall -g -c server.c
	@gcc -Wall -g -c common_utils.c
	@gcc -Wall -g -c packer.c
	@gcc -Wall -g -c receiver.c
	@gcc -Wall -g -c sender.c
	@gcc -o server server.o common_utils.o
	@gcc -o packer packer.o common_utils.o
	@gcc -o receiver receiver.o common_utils.o
	@gcc -o sender sender.o common_utils.o

run:
	./server

clean:
	@rm -f *.o
	@rm -f server packer receiver sender
