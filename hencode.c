/* decompresses a huffman-encoded file */

#include <arpa/inet.h>  /* htonl for linux??? */
#include <fcntl.h>
#include "huffman.h"  /* this also includes "list.h" and "htree.h" */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>  /* unix i/o */

/* NUM_CHARS is the number of possible values for a char */
#define NUM_CHARS 256

/* & MSB_MASK to zero out everything except the first bit in a char */
#define MSB_MASK 0x80


void countChars(int infile, unsigned int *freqTable) {
    unsigned char *nextChar;
    ReadBuf *rbuf = readBufCreate(infile);

    while (readFromBuf(infile, nextChar, rbuf) == 0) {
        freqTable[nextChar]++;
    }

    readBufDestroy(rbuf);
}


void encode(int infile, int outfile, char **codes) {
    unsigned char nextChar;
    unsigned char writeByte;
    int writeByteIndex;
    ReadBuf *rbuf = readBufCreate(infile);
    WriteBuf *wbuf = write(outfile);

    while ((nextChar = readFromBuf(infile, &nextChar, rbuf)) == 0) {
        writeCode()
    }
}


/* initializes infile and outfile */
void parseArgs(int argc, char *argv[], int *infile, int *outfile) {
    /* no more than two args, argc can't be more than 3 */
    if (argc > 3) {
        fprintf(stderr, "hencode: extra operand `%s`\nusage: hencode infile [outfile]\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    /* argv[1] infile is required */
    if (argc < 2) {
        fprintf(stderr, "usage: hencode infile [outfile]\n");
        exit(EXIT_FAILURE);
    }

    /* infile will be the first arg */
    *infile = open(argv[1], O_RDONLY);

    /* outfile is the second arg if provided, otherwise stdout */
    if (argc == 3) {
        *outfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    }
    else {
        *outfile = STDOUT_FILENO;
    }

    /* error messages if open() didn't work */
    if (*infile < 0) {
        fprintf(stderr, "%s: No such file or directory\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    if (*outfile < 0) {
        fprintf(stderr, "%s: No such file or directory\n", argv[3]);
        exit(EXIT_FAILURE);
    }
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

    char **codes;

    List *list;
    HNode *node1;
    HNode *node2;
    HNode *newNode;


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


    /* count all chars in infile and put them in freqTable */
    countChars(infile, freqTable);


    /* put freqTable into sorted linked list of huffman trees */
    list = listCreate();
    for (i = 0; i < NUM_CHARS; i++) {
        if (freqTable[i] > 0) {
            node1 = htreeCreate(freqTable[i], i);
            listInsert(list, node1);
        }
    }


    /* combine and re-insert htrees until we have only one */
    while (list->size > 1) {
        node1 = listRemoveHead(list);
        node2 = listRemoveHead(list);

        newNode = htreeCreate(node1->freq + node2->freq,
                              node1->chr < node2->chr ? node1->chr : node2->chr);
        newNode->left = node1;
        newNode->right = node2;
        listInsert2(list, newNode);
    }


    /* traverse tree (list->head->data) to get codes for each character */
    codes = (char **) malloc(sizeof(char *) * NUM_CHARS);
    for (i = 0; i < NUM_CHARS; i++) {  /* initialize codes to NULL */
        codes[i] = NULL;
    }


    createCodes(list->head->data, codes, NULL, 0);\


    encode(infile, outfile, codes);




    /* cleanup */
    free(freqTable);
    for (i = 0; i < NUM_CHARS; i++) {
        if (codes[i] != NULL) {
            free(codes[i]);
        }
    }
    listDestroy(list);
    close(infile);
    close(outfile);
    return 0;
}

