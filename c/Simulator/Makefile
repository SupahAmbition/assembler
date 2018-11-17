CC=gcc
CFLAGS=-pipe -g 


all: simulator.o 
	$(CC) $(CFLAGS) simulator.o  -o simulator 

simulator.o: simulator.c
	$(CC) $(CFLAGS) -c simulator.c  

clean: 
	rm -f *.o 
	rm -f simulator 
