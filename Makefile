CC = gcc
CFLAGS = -Wall -pedantic -g -Og


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


tests: hencode hdecode
	make empty
	make one
	make two
	make frank
	make pride
	make dick
	make war


empty: hencode hdecode
	./hencode tests/empty tests/alsoempty
	diff tests/empty tests/alsoempty
	./hdecode tests/alsoempty tests/myempty
	diff tests/empty tests/myempty
	rm tests/alsoempty tests/myempty

one: hencode hdecode
	./hencode tests/singlerepeatingchar tests/coderep
	./hdecode tests/coderep tests/myrep
	diff tests/singlerepeatingchar tests/myrep
	rm tests/coderep tests/myrep

two: hencode hdecode
	./hencode tests/test2 tests/code2
	./hdecode tests/code2 tests/out2
	diff tests/test2 tests/out2
	rm tests/code2 tests/out2

frank: hencode hdecode
	./hencode tests/frank tests/agentfrank
	./hdecode tests/agentfrank tests/myfrank
	diff tests/frank tests/myfrank
	rm tests/agentfrank tests/myfrank

pride: hencode hdecode
	./hencode tests/pride tests/secretpride
	./hdecode tests/secretpride tests/mypride
	diff tests/pride tests/mypride
	rm tests/secretpride tests/mypride

dick: hencode hdecode
	./hencode tests/dick tests/secretdick
	./hdecode tests/secretdick tests/mydick
	diff tests/dick tests/mydick
	rm tests/secretdick tests/mydick

war: hencode hdecode
	./hencode tests/warandpeace tests/codewar
	./hdecode tests/codewar tests/mywar
	diff tests/warandpeace tests/mywar
	rm tests/codewar tests/mywar


clean:
	rm -f *.o

server:
	ssh acheun29@unix1.csc.calpoly.edu

upload:
	scp -r ../csc357-asgn3 acheun29@unix1.csc.calpoly.edu:csc357

download:
	scp -r acheun29@unix1.csc.calpoly.edu:csc357/csc357-asgn3 /Users/andrewcheung/Documents/Cal\ Poly\ Stuff/csc357

