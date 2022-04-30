/* huffman tree */

#include "htree.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_CODE_LEN 40


int createCodes(HNode *htree, char **codes, char *path, int len) {
    int i;
    char *leftPath;
    char *rightPath;

    if (htree == NULL) {
        return 0;
    }

    /* found a leaf, add code to codes and return 1 */
    if (htree->left == NULL && htree->right == NULL) {
        path[len] = '\0';
        codes[(unsigned char) htree->chr] = path;
        return 1;
    }

    leftPath = (char *) malloc(sizeof(char) * (len + 2));
    rightPath = (char *) malloc(sizeof(char) * (len + 2));
    if (leftPath == NULL || rightPath == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* leftPath is path + '0', rightPath is path + '1' */
    for (i = 0; i < len; i++) {
        leftPath[i] = path[i];
        rightPath[i] = path[i];
    }
    leftPath[i] = '0';
    rightPath[i] = '1';
    len++;

    /* createCodes left and right. free if it wasn't a leaf */
    if (!createCodes(htree->left, codes, leftPath, len)) {
        free(leftPath);
    }
    if (!createCodes(htree->right, codes, rightPath, len)) {
        free(rightPath);
    }

    /* let previous level of recursion know that this path can be freed */
    return 0;
}


/* compares nodes based on freq, then chr. returns positive if
 * node1 > node2, negative if node1 < node2, 0 otherwise */
int hnodeCompare(HNode *node1, HNode *node2) {
    if (node1->freq > node2->freq) {
        return 1;
    }

    if (node1->freq < node2->freq) {
        return -1;
    }

    if ((unsigned char) node1->chr > (unsigned char) node2->chr) {
        return 1;
    }

    if ((unsigned char) node1->chr < (unsigned char) node2->chr) {
        return -1;
    }

    return 0;
}


HNode *htreeCreate(int freq, char chr) {
    HNode *htree = (HNode *) malloc(sizeof(HNode));

    if (htree == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    htree->chr = chr;
    htree->freq = freq;
    htree->left = NULL;
    htree->right = NULL;

    return htree;
}


void htreeDestroy(HNode *htree) {
    if (htree == NULL) {
        return;
    }

    htreeDestroy(htree->left);
    htreeDestroy(htree->right);
    free(htree);
}

