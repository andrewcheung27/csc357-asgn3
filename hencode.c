/* compresses a file with the huffman algorithm */

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


/* reads infile and makes frequency table */
void countChars(int infile, unsigned int *freqTable) {
    unsigned char nextChar;
    ReadBuf *rbuf = readBufCreate(infile);

    while (readFromBuf(&nextChar, rbuf) == 0) {
        freqTable[nextChar]++;
    }

    readBufDestroy(rbuf);
    lseek(infile, 0, SEEK_SET);  /* set offset back to 0 */
}


/* writes metadata for compressed file */
void writeHeader(int outfile, unsigned int *freqTable) {
    int i;
    int uniqueChars;
    unsigned char c;
    unsigned int freq;

    /* first byte of header: number of unique characters - 1 */
    uniqueChars = 0;
    for (i = 0; i < NUM_CHARS; i++) {
        if (freqTable[i] > 0) {
            uniqueChars++;
        }
    }
    c = (unsigned char) (uniqueChars - 1);
    write(outfile, &c, 1);

    /* write metadata for each unique char */
    for (i = 0; i < NUM_CHARS; i++) {
        if (freqTable[i] > 0) {
            c = (unsigned char) i;
            write(outfile, &c, 1);  /* 1 byte: character */
            freq = htonl(freqTable[i]);  /* network byte order */
            write(outfile, &freq, 4);  /* 4 bytes: freq */
        }
    }
}


/* writes string version of code to outfile as bits */
void writeCode(char *strCode, unsigned char *byte,
               unsigned int *index, WriteBuf *wbuf) {
    int i = 0;  /* indexing strCode */

    while (strCode[i]) {
        /* write 1 to byte for a 1, otherwise it will remain a 0 */
        if (strCode[i] == '1') {
            *byte = *byte | (MSB_MASK >> *index);
        }

        *index = (*index + 1) % 8;  /* indexing byte */

        /* if byte is finished, write it and reset to 0000 0000 */
        if (*index == 0) {
            writeToBuf((char) *byte, wbuf);
            *byte = 0;
        }

        i++;
    }
}


/* reads infile, writing encoded version of each character to outfile */
void encode(int infile, int outfile, char **codes) {
    unsigned char nextChar;
    unsigned char writeByte;
    unsigned int writeByteIndex;
    ReadBuf *rbuf = readBufCreate(infile);
    WriteBuf *wbuf = writeBufCreate(outfile);

    /* read infile chars, find their string code,
     * and write to outfile as bits */
    writeByte = 0;
    writeByteIndex = 0;
    while (readFromBuf(&nextChar, rbuf) == 0) {
        writeCode(codes[nextChar], &writeByte,&writeByteIndex, wbuf);
    }

    /* write the last byte if it doesn't have all 8 bits filled,
     * empty spaces are padded with zeroes */
    if (writeByteIndex) {
        writeToBuf((char) writeByte, wbuf);
    }

    /* write anything left in the buffer */
    if (wbuf->size) {
        write(outfile, wbuf->buf, wbuf->size);
    }

    /* cleanup */
    readBufDestroy(rbuf);
    writeBufDestroy(wbuf);
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
        fprintf(stderr, "%s: No such file or directory\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (*outfile < 0) {
        fprintf(stderr, "%s: No such file or directory\n", argv[2]);
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[]) {
    int infile;
    int outfile;

    unsigned int *freqTable;
    char **codes;
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

    /* count all chars in infile and put them in freqTable */
    countChars(infile, freqTable);

    list = constructHTree(freqTable, NUM_CHARS);

    /* traverse tree (list->head->data) to get codes for each character */
    codes = (char **) malloc(sizeof(char *) * NUM_CHARS);
    for (i = 0; i < NUM_CHARS; i++) {  /* initialize codes to NULL */
        codes[i] = NULL;
    }

    /* traverse tree to get codes for each char */
    createCodes(list->head->data, codes, NULL, 0);

    /* encode freqTable and write to outfile */
    writeHeader(outfile, freqTable);

    /* encode the rest of infile */
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

