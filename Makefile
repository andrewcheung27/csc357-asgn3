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


d2: hdecode
	~pn-cs357/demos/hencode test2 peencode2
	./hdecode peencode2 out2


clean:
	rm -f *.o

server:
	ssh acheun29@unix1.csc.calpoly.edu

upload:
	scp -r ../csc357-asgn3 acheun29@unix1.csc.calpoly.edu:csc357

download:
	scp -r acheun29@unix1.csc.calpoly.edu:csc357/csc357-asgn3 /Users/andrewcheung/Documents/Cal\ Poly\ Stuff/csc357

