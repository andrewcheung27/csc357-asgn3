CC = gcc
CFLAGS = -Wall -pedantic -g -O3


all: hencode hdecode
	make hencode
	make hdecode


hencode: hencode.o huffman.o list.o htree.o
	$(CC) $(CFLAGS) -o hencode hencode.o huffman.o list.o htree.o


hdecode: hdecode.o huffman.o list.o htree.o
	$(CC) $(CFLAGS) -o hdecode hdecode.o huffman.o list.o htree.o


hencode.o: hencode.c
	$(CC) $(CFLAGS) -c -o hencode.o hencode.c
hdecode.o: hdecode.c
	$(CC) $(CFLAGS) -c -o hdecode.o hdecode.c
huffman.o: huffman.c
	$(CC) $(CFLAGS) -c -o huffman.o huffman.c
list.o: list.c
	$(CC) $(CFLAGS) -c -o list.o list.c
htree.o: htree.c
	$(CC) $(CFLAGS) -c -o htree.o htree.c


t: tests/test.c
	$(CC) $(CFLAGS) -o t tests/test.c
two: hencode hdecode
	./hencode tests/test2 tests/code2
	./hdecode tests/code2 tests/out2
war: hencode hdecode
	./hencode tests/warandpeace tests/codewar
	./hdecode tests/codewar tests/mywar


clean:
	rm -f *.o

server:
	ssh acheun29@unix1.csc.calpoly.edu

upload:
	scp -r ../csc357-asgn3 acheun29@unix1.csc.calpoly.edu:csc357

download:
	scp -r acheun29@unix1.csc.calpoly.edu:csc357/csc357-asgn3 /Users/andrewcheung/Documents/Cal\ Poly\ Stuff/csc357

