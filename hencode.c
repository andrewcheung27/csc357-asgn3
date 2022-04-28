/* encodes a file using huffman compression */

#include <getopt.h>
#include "list.h"
#include <stdlib.h>
#include <stdio.h>

#define NUM_CHARS 256


int main(int argc, char *argv[]) {
    int option;
    int i;
    FILE *file;

    int *charHistogram;
    char **codes;
    int codesLen = NUM_CHARS;
    HNode *node1;
    HNode *node2;
    HNode *newNode;
    List *list = listCreate();

    /* use getopt to make sure there are no options */
    while ((option = getopt(argc, argv, ":")) != -1) {
        if (option == '?') {
            fprintf(stderr,
                    "No options accepted.\nusage: hencode infile [outfile]\n");
            exit(EXIT_FAILURE);
        }
    }

    /* make sure a filename is provided */
    if (optind == argc) {
        fprintf(stderr, "Filename required.\nusage: hencode infile [outfile]\n");
        exit(EXIT_FAILURE);
    }

    /* get filename from args and open file */
    file = fopen(argv[optind++], "r");
    if (file == NULL) {
        fprintf(stderr,
                "%s: No such file or directory\nusage: hencode infile [outfile]\n",
                argv[optind]);
        exit(EXIT_FAILURE);
    }

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

