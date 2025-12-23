prog: main.o protocols.o
	@gcc main.o protocols.o -o prog

main.o: main.c protocols.h
	@gcc -c main.c -o main.o

protocols.o: protocols.c protocols.h
	@gcc -c protocols.c -o protocols.o

clean:
	@rm -f *.o prog