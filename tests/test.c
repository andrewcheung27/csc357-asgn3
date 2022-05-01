#include <arpa/inet.h>  /* htonl/ntohl for linux??? */
#include <fcntl.h>
#include "huffman.h"  /* this also includes "list.h" and "htree.h" */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>  /* unix i/o */

/* NUM_CHARS is the number of possible values for a char */
#define NUM_CHARS 256

/* & ONE_BIT_MASK to zero out everything except the first bit in a char */
#define ONE_BIT_MASK 0x80



/* TEST WRITECODE BY SENDING RANDOM CODES AND PRINTING SHIT??? */
void writeCode(int outfile, char *strCode, unsigned char *byte,
               unsigned int *index, WriteBuf *wbuf) {
    int i = 0;  /* indexing strCode */

    while (strCode[i]) {
        if (strCode[i] == '1') {
            *byte = *byte | (ONE_BIT_MASK >> *index);
        }

        *index = (*index + 1) % 8;

        if (*index == 0) {
            /* printf("\nwriting byte 0x%x\n", *byte); */
            writeToBuf(outfile, (char) *byte, wbuf);
            *byte = 0;
        }

        i++;
    }
}


int main (void) {
    int outfile = STDOUT_FILENO;
    int maxLen = 100;
    char *strCode = (char *) malloc(sizeof(char) * maxLen);
    if (strCode == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(strCode, "01100111011001010");
    unsigned char byte;
    unsigned int index;
    WriteBuf *wbuf = writeBufCreate(outfile);

    byte = 0;
    index = 0;
    writeCode(outfile, strCode, &byte, &index, wbuf);

    if (wbuf->size) {
        write(outfile, wbuf->buf, wbuf->size);
    }

    printf("\ntest.c done\n");
    return 0;
}


