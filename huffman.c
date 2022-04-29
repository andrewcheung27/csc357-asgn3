/* huffman compression functions shared between hencode and hdecode */

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>


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


void writeBuf(char c, char *buf, unsigned int *size, unsigned int *capacity) {
    /* resize buf if needed */
    if (*size >= *capacity) {
        *capacity *= 2;
        buf = (char *) realloc(buf, sizeof(char) * (*capacity));
        if (buf == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }

    buf[*size] = c;
    *size += 1;
}
