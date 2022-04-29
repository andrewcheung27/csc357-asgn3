/* decompresses a huffman-encoded file */

#include <fcntl.h>
#include "huffman.h"
#include "list.h"  /* this also includes htree.h */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* NUM_CHARS is the number of possible values for a char */
#define NUM_CHARS 256
/* & MSB_MASK to zero out everything except the first bit in a char */
#define MSB_MASK 0x80


static int nextBit(char byte) {
    if ((byte & MSB_MASK) < 0) {
        return 1;
    }
    return 0;
}


static void decode(HNode *htree, int infile, int outfile) {
    char nextByte;
    HNode *cur;

    lseek(infile, 0, SEEK_SET);
    while (read(infile, &nextByte, 1) == 1) {

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

    HNode *htree;

    /* parse args */
    while (argc-- > 1) {
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
            }
        }

        /* first optional arg is name if infile */
        if (argc == 2) {
            /* read from stdin if first arg is "-" */
            if (!strcmp(argv[1], "-"))  {
                infile = STDIN_FILENO;
            }
            else {
                infile = open(argv[1], O_RDONLY);
            }
        }
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
    while ((uniqueChars--) > 0) {
        read(infile, &nextChar, 1);  /* 1 byte: character */
        read(infile, &freq, 4);  /* 4 bytes: frequency of the character */
        freqTable[nextChar] = freq;
        totalFreq += freq;
    }

    /* make huffman tree for decoding */
    htree = constructHTree(freqTable, NUM_CHARS);

    decode(htree, infile, outfile);


    /* lab 3 shit */

    /* traverse tree (list->head->data) to get codes for each character */
    codes = (char **) malloc(sizeof(char *) * codesLen);
    for (i = 0; i < codesLen; i++) {
        codes[i] = NULL;
    }

    createCodes(list->head->data, codes, NULL, 0);
    printCodes(codes, codesLen);

    /* cleanup */
    for (i = 0; i < codesLen; i++) {
        if (codes[i] != NULL) {
            free(codes[i]);
        }
    }
    free(codes);
    listDestroy(list);
    return 0;
}

