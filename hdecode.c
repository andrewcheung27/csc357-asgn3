/* decompresses a huffman-encoded file */

#include <arpa/inet.h>  /* htonl/ntohl for linux??? */
#include <fcntl.h>
#include "huffman.h"  /* this also includes "list.h" and "htree.h" */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>  /* unix i/o */

/* NUM_CHARS is the number of possible values for a char */
#define NUM_CHARS 256

/* 1000 0000 */
#define MSB_MASK 0x80


/* returns the first bit of a char as an int */
int getFirstBit(char byte) {
    if ((byte & MSB_MASK) == MSB_MASK) {
        return 1;
    }

    return 0;
}


/* returns the next bit of infile as an int, or -1 for end of file */
int getNextBit(unsigned char *byte, int *index, ReadBuf *rbuf) {
    int result;

    if (*index == 0) {
        /* readBuf() sets nextByte to next byte of infile if there is one,
         * returns -1 for end of file */
        if (readFromBuf(byte, rbuf) < 0) {
            return -1;
        }
    }

    result = getFirstBit(*byte << *index);
    *index = (*index + 1) % 8;
    return result;
}


/* uses infile to traverse huffman tree, writing encoded file to outfile */
void decode(int infile, int outfile, HNode *htree, 
        unsigned long totalFreq) {
    unsigned char byte;  /* stores current byte whose bits are being read */
    int bit;  /* current bit as an int, becomes -1 at EOF */
    int index = 0;  /* current index in the current byte */
    HNode *node = htree;

    /* buffered read and write */
    ReadBuf *rbuf = readBufCreate(infile);
    WriteBuf *wbuf = writeBufCreate(outfile);

    /* traverse huffman tree */
    while (totalFreq > 0) {
        bit = getNextBit(&byte, &index, rbuf);

        /* error, expecting more chars than there actually are.
         * might as well finish up the program though */
        if (bit == -1) {
            fprintf(stderr, "encoded file may not be valid\n");
            break;
        }

        if (bit == 0) {
            node = node->left;
        }
        else {
            node = node->right;
        }
        /* write character if this is a leaf */
        if (node->left == NULL && node->right == NULL) {
            writeToBuf(node->chr, wbuf);
            totalFreq--;
            node = htree;
        }
    }

    /* write anything left in the buffer */
    writeBufFlush(wbuf);

    /* cleanup */
    readBufDestroy(rbuf);
    writeBufDestroy(wbuf);
}


/* special decoding if there is only one unique char */
void oneCharDecode(int outfile, unsigned char c, int freq) {
    WriteBuf *wbuf = writeBufCreate(outfile);

    while (freq > 0) {
        writeToBuf(c, wbuf);
        freq--;
    }

    /* write anything left in the buffer */
    writeBufFlush(wbuf);
}


/* initializes infile and outfile */
void parseArgs(int argc, char *argv[], int *infile, int *outfile) {
    /* no more than two args, argc can't be more than 3 */
    if (argc > 3) {
        fprintf(stderr, "usage: hdecode [(infile | -)] [outfile]\n");
        exit(EXIT_FAILURE);
    }

    /* first optional arg is name of infile,
     * default is stdin */
    if (argc == 2 || argc == 3) {
        /* read from stdin if first arg is "-" */
        if (!strcmp(argv[1], "-"))  {
            *infile = STDIN_FILENO;
        }
        else {
            *infile = open(argv[1], O_RDONLY);
        }
    }
    else {
        *infile = STDIN_FILENO;
    }

    /* second optional arg is name of outfile,
     * default is stdout */
    if (argc == 3) {
        *outfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    }
    else {
        *outfile = STDOUT_FILENO;
    }

    /* error messages if open() didn't work */
    if (*infile < 0) {
        fprintf(stderr, "usage: hdecode [(infile | -)] [outfile]\n");
        exit(EXIT_FAILURE);
    }
    if (*outfile < 0) {
        fprintf(stderr, "usage: hdecode [(infile | -)] [outfile]\n");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[]) {
    int infile;
    int outfile;

    int uniqueChars;
    unsigned char nextChar;
    unsigned int freq;
    unsigned long totalFreq;
    unsigned int *freqTable;
    List *list;
    int i;

    /* parse args to initialize infile and outfile */
    parseArgs(argc, argv, &infile, &outfile);

    /* write nothing if infile is empty */
    if (fileSize(infile) == 0) {
        return 0;
    }

    /* freqTable[c] will have the frequency of character c in the infile */
    freqTable = (unsigned int *) malloc(sizeof(int) * NUM_CHARS);
    if (freqTable == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < NUM_CHARS; i++) {  /* initialize frequencies to 0 */
        freqTable[i] = 0;
    }

    /* read the first byte, which contains the number of unique chars - 1 */
    if (read(infile, &nextChar, 1) == -1) {
        perror("read");
    }

    /* increment uniqueChars because it was the number of unique chars - 1,
     * so that it could fit in a byte */
    uniqueChars = nextChar;
    uniqueChars++;  /* transfer to int, then increment, so no overflow */
    totalFreq = 0;
    /* read compressed file header into freqTable */
    i = uniqueChars;
    while (i-- > 0) {
        if (read(infile, &nextChar, 1) == -1) { /* 1 byte: character */
            perror("read");
        }
        if (read(infile, &freq, 4) == -1) {  /* 4 bytes: frequency */
            perror("read");
        }
        freq = ntohl(freq);  /* convert to host byte order */
        freqTable[nextChar] = freq;
        totalFreq += freq;
    }

    /* special case for files with one unique char */
    if (uniqueChars == 1) {
        oneCharDecode(outfile, nextChar, totalFreq);

        free(freqTable);
        close(infile);
        close(outfile);
        return 0;
    }

    /* create huffman tree */
    list = constructHTree(freqTable, NUM_CHARS);
    if (list == NULL || list->head == NULL) {
        fprintf(stderr, "file could not be decoded\n");
        exit(EXIT_FAILURE);
    }

    /* decode and write to outfile */
    decode(infile, outfile, list->head->data, totalFreq);

    /* cleanup */
    free(freqTable);
    listDestroy(list);
    close(infile);
    close(outfile);
    return 0;
}

