/* huffman compression functions shared between hencode and hdecode */

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define DEFAULT_BUF_CAPACITY 4096


/* sets buffer capacity to the file's block size if possible,
 * or to the default otherwise */
static int bufCapacity(int fd) {
    struct stat infileStat;
    int capacity;

    capacity = DEFAULT_BUF_CAPACITY;
    if (fstat(fd, &infileStat) == 0) {
        capacity = infileStat.st_blksize;
    }

    return capacity;
}


List *constructHTree(unsigned int *freqTable, int size) {
    List *list;
    HNode *node1;
    HNode *node2;
    HNode *newNode;
    int i;

    /* create list of huffman trees from freqTable */
    list = listCreate();
    for (i = 0; i < size; i++) {
        if (freqTable[i] > 0) {
            node1 = htreeCreate(freqTable[i], i);
            listInsert(list, node1);
        }
    }

    /* combine elements of list into the huffman tree */
    while (list->size > 1) {
        node1 = listRemoveHead(list);
        node2 = listRemoveHead(list);

        newNode = htreeCreate(node1->freq + node2->freq,
                              node1->chr < node2->chr ? node1->chr : node2->chr);
        newNode->left = node1;
        newNode->right = node2;
        listInsert2(list, newNode);
    }

    return list;
}


ReadBuf *readBufCreate(int infile) {
    ReadBuf *rbuf = (ReadBuf *) malloc(sizeof(ReadBuf));
    if (rbuf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    rbuf->index = 0;
    rbuf->size = 0;
    rbuf->capacity = bufCapacity(infile);

    rbuf->buf = (char *) malloc(sizeof(char) * rbuf->capacity);
    if (rbuf->buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    return rbuf;
}


void readBufDestroy(ReadBuf *rbuf) {
    free(rbuf->buf);
    free(rbuf);
}


/* puts the next byte of infile into nextByte. returns 0 if something was read,
 * or -1 if the end of the file has been reached */
int readFromBuf(int infile, char *nextByte, ReadBuf *rbuf) {
    /* if we're at the end of the buffer,
     * read next chunk from infile into the buffer */
    if (rbuf->index >= rbuf->size) {
        rbuf->size = read(infile, rbuf->buf, rbuf->capacity);
        if (rbuf->size == 0) {  /* return -1 for end of file */
            return -1;
        }
        rbuf->index = 0;
    }

    /* set nextByte to the next byte in the buffer, return 0 for success */
    *nextByte = (rbuf->buf)[rbuf->index];
    rbuf->index++;
    return 0;
}


WriteBuf *writeBufCreate(int outfile) {
    WriteBuf *wbuf = (WriteBuf *) malloc(sizeof(WriteBuf));
    if (wbuf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    wbuf->size = 0;
    wbuf->capacity = bufCapacity(outfile);

    wbuf->buf = (char *) malloc(sizeof(char) * wbuf->capacity);
    if (wbuf->buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    return wbuf;
}


void writeBufDestroy(WriteBuf *wbuf) {
    free(wbuf->buf);
    free(wbuf);
}


void writeToBuf(int outfile, char c, WriteBuf *wbuf) {
    /* write to outfile when buffer is full */
    if (wbuf->size >= wbuf->capacity) {
        write(outfile, wbuf->buf, wbuf->size);
        wbuf->size = 0;
    }

    (wbuf->buf)[wbuf->size] = c;
    wbuf->size++;
}
