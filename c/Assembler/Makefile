CC=gcc
CFLAGS=-pipe -g 


all: assembler.o 
	$(CC) $(CFLAGS) assembler.o  -o assembler 

assembler.o: assembler.c assembler.h 
	$(CC) $(CFLAGS) -c assembler.c  

test: assembler.o 
	$(CC) $(CFLAGS) assembler.o -o assembler 
	./assembler -i ./tests/test01 
	./assembler -i ./tests/test02
	./assembler -i ./tests/test03
	./assembler -i ./tests/test04
	./assembler -i ./tests/test06
	./assembler -i ./tests/test06
	./assembler -i ./tests/test07
	./assembler -i ./tests/test08
	./assembler -i ./tests/test09
	./assembler -i ./tests/test10

clean: 
	rm -f *.o 
	rm -f assembler
