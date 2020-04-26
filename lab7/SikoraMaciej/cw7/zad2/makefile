all:
	make clean
	clear
	@make compile -s
	
compile:
	@gcc -Wall -g -c server.c -lpthread -lrt
	@gcc -Wall -g -c common_utils.c -lpthread -lrt
	@gcc -Wall -g -c packer.c -lpthread -lrt
	@gcc -Wall -g -c receiver.c -lpthread -lrt
	@gcc -Wall -g -c sender.c -lpthread -lrt
	@gcc -o server server.o common_utils.o -lpthread -lrt
	@gcc -o packer packer.o common_utils.o -lpthread -lrt
	@gcc -o receiver receiver.o common_utils.o -lpthread -lrt
	@gcc -o sender sender.o common_utils.o -lpthread -lrt

run:
	./server

clean:
	@rm -f *.o
	@rm -f server packer receiver sender
