CC = gcc
CFLAGS = -Wall -pedantic -g -O3


all: hencode hdecode
	make hencode
	make hdecode


hencode: hencode.o huffman.o list.o htree.o
	$(CC) $(CFLAGS) -o hencode hencode.o huffman.o list.o htree.o


hdecode: hdecode.o huffman.o list.o htree.o
	$(CC) $(CFLAGS) -o hdecode hdecode.o huffman.o list.o htree.o


huffman.o: huffman.c
	$(CC) $(CFLAGS) -c -o huffman.o huffman.c
list.o: list.c
	$(CC) $(CFLAGS) -c -o list.o list.c
htree.o: htree.c
	$(CC) $(CFLAGS) -c -o htree.o htree.c


t: test.c
	$(CC) $(CFLAGS) -o t test.c

dtests:
	./htable test1 > out1
	~pn-cs357/demos/htable test1 > pnout1
	diff out1 pnout1
	./htable test2 > out2
	~pn-cs357/demos/htable test2 > pnout2
	diff out2 pnout2
	./htable words > outwords
	~pn-cs357/demos/htable words > pnoutwords
	diff outwords pnoutwords
	rm out1 pnout1 out2 pnout2 outwords pnoutwords


clean:
	rm -f *.o

server:
	ssh acheun29@unix1.csc.calpoly.edu

upload:
	scp -r ../csc357-asgn3 acheun29@unix1.csc.calpoly.edu:csc357

download:
	scp -r acheun29@unix1.csc.calpoly.edu:csc357/csc357-asgn3 /Users/andrewcheung/Documents/Cal\ Poly\ Stuff/csc357

