/* decompresses a huffman-encoded file */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    int infile;
    int outfile;

    unsigned char uniqueChars;

    int *charHistogram;
    char **codes;
    int codesLen = NUM_CHARS;
    HNode *node1;
    HNode *node2;
    HNode *newNode;
    List *list = listCreate();

    /* parse args */
    while (argc-- > 1) {
        /* no more than two args, argc can't be more than 3 */
        if (argc > 3) {
            fprintf(stderr, "hdecode: extra operand `%s`", argv[3]);
            exit(EXIT_FAILURE);
        }

        /* second optional arg is name of outfile */
        if (argc == 3) {
            outfile = open(argv[2], O_RDONLY);
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

    /* lab 3 shit */
    charHistogram = (int *) malloc(sizeof(int) * NUM_CHARS);
    if (charHistogram == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    /* initialize histogram values to 0 */
    for (i = 0; i < NUM_CHARS; i++) {
        charHistogram[i] = 0;
    }

    /* count characters, insert a huffman tree for each unique character
     * into the sorted list */
    countChars(file, charHistogram);
    fclose(file);
    for (i = 0; i < NUM_CHARS; i++) {
        if (charHistogram[i] > 0) {
            node1 = htreeCreate(charHistogram[i], i);
            listInsert(list, node1);
        }
    }
    free(charHistogram);

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

