#include "list.h"  /* this also includes htree.h */

typedef struct ReadBuf {
    char *buf;
    int index;
    int size;
    int capacity;
} ReadBuf;

typedef struct WriteBuf {
    char *buf;
    int size;
    int capacity;
} WriteBuf;


List *constructHTree(unsigned int *freqTable, int size);

ReadBuf *readBufCreate(int infile);

int readFromBuf(int infile, char *nextByte, ReadBuf *rbuf);

WriteBuf *writeBufCreate(int outfile);

void writeToBuf(int outfile, char c, WriteBuf *wbuf);
