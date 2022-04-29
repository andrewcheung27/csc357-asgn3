/* decompresses a huffman-encoded file */

#include <arpa/inet.h>  /* htonl */
#include <fcntl.h>
#include "huffman.h"  /* this also includes "list.h" and "htree.h" */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* NUM_CHARS is the number of possible values for a char */
#define NUM_CHARS 256
/* & MSB_MASK to zero out everything except the first bit in a char */
#define MSB_MASK 0x80
#define BUF_CAPACITY 4096


static int getFirstBit(char byte) {
    if ((byte & MSB_MASK) < 0) {
        return 1;
    }
    return 0;
}

static int getNextBit(int infile, char *nextByte, int *byteIndex) {
    int result;

    if (*byteIndex == 0) {
        /* read next byte in file, return -1 if nothing can be read */
        if (read(infile, nextByte, 1) < 1) {
            return -1;
        }
        *nextByte = htonl(*nextByte);
    }

    result = getFirstBit(*nextByte << *byteIndex);

    *byteIndex = (*byteIndex + 1) % 8;

    return result;
}


static void decode(HNode *htree, int infile, int outfile, unsigned long totalFreq) {
    char nextByte;
    int byteIndex;
    int nextBit;
    HNode *node;

    char *buf;
    unsigned int bufSize;
    unsigned int bufCapacity;

    byteIndex = 0;
    bufSize = 0;
    bufCapacity = BUF_CAPACITY;
    buf = (char *) malloc(sizeof(char) * bufCapacity);
    node = htree;
    while (totalFreq > 0 && (nextBit = getNextBit(infile, &nextByte, &byteIndex)) != -1) {
        if (nextBit == 0) {
            node = node->left;
        }
        else {
            node = node->right;
        }
        if (node->left == NULL && node->right == NULL) {
            writeBuf(node->chr, buf, &bufSize, &bufCapacity);
            totalFreq--;
            node = htree;
        }
    }

    write(outfile, buf, bufSize);
}


int main(int argc, char *argv[]) {
    int infile;
    int outfile;

    int i;
    unsigned char uniqueChars;
    unsigned char nextChar;
    unsigned int freq;
    unsigned long totalFreq;
    unsigned int *freqTable;
    List *list;


    /* parse args */

    /* no more than two args, argc can't be more than 3 */
    if (argc > 3) {
        fprintf(stderr, "hdecode: extra operand `%s`", argv[3]);
        exit(EXIT_FAILURE);
    }
    /* second optional arg is name of outfile */
    if (argc == 3) {
        outfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC);
        if (outfile < 0) {
            fprintf(stderr, "%s: No such file or directory\n", argv[3]);
            exit(EXIT_FAILURE);
        }
    }
    else {
        outfile = STDOUT_FILENO;
    }
    /* first optional arg is name of infile */
    if (argc == 3 || argc == 2) {
        /* read from stdin if first arg is "-" */
        if (!strcmp(argv[1], "-"))  {
            infile = STDIN_FILENO;
        }
        else {
            infile = open(argv[1], O_RDONLY);
            if (infile < 0) {
                fprintf(stderr, "%s: No such file or directory\n", argv[2]);
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        infile = STDIN_FILENO;
    }


    /* read the first byte, which contains the number of unique chars - 1.
     * if read() returns 0, then 0 bytes were read, so this is an empty file.
     * if that is the case, nothing will be written to outfile. */
    if (read(infile, &uniqueChars, 1) < 1) {
        return 0;
    }


    /* freqTable[c] will have the frequency of character c in the infile */
    freqTable = (unsigned int *) malloc(sizeof(unsigned int) * NUM_CHARS);
    if (freqTable == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < NUM_CHARS; i++) {  /* initialize frequencies to 0 */
        freqTable[i] = 0;
    }


    /* increment uniqueChars because it was the number of unique chars - 1,
     * so that it could fit in a byte */
    uniqueChars++;
    /* read compressed file header into freqTable */
    totalFreq = 0;
    while ((uniqueChars--) > 0) {
        read(infile, &nextChar, 1);  /* 1 byte: character */
        read(infile, &freq, 4);  /* 4 bytes: frequency of the character */
        freq = htonl(freq);  /* network byte order */
        freqTable[nextChar] = freq;
        totalFreq += freq;
    }


    list = constructHTree(freqTable, NUM_CHARS);


    decode(list->head->data, infile, outfile, totalFreq);


    /* cleanup */
    free(freqTable);
    listDestroy(list);
    return 0;
}

