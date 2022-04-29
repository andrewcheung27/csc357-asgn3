/* huffman compression functions shared between hencode and hdecode */

#include "list.h"  /* this also includes htree.h */


HNode *constructHTree(unsigned int *freqTable, size_t size) {
    size_t i;

    /* create list of huffman trees from freqTable */
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

    return list->head->data;
}
