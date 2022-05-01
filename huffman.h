#include "list.h"  /* this also includes htree.h */

typedef struct ReadBuf {
    char *buf;
    int fd;
    int index;
    int size;
    int capacity;
} ReadBuf;

typedef struct WriteBuf {
    char *buf;
    int fd;
    int size;
    int capacity;
} WriteBuf;


List *constructHTree(unsigned int *freqTable, int size);

int fileSize(int fd);

ReadBuf *readBufCreate(int infile);

void readBufDestroy(ReadBuf *rbuf);

int readFromBuf(unsigned char *nextByte, ReadBuf *rbuf);

WriteBuf *writeBufCreate(int outfile);

void writeBufDestroy(WriteBuf *wbuf);

void writeToBuf(char c, WriteBuf *wbuf);

