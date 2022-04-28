/* sorted linked list of HNodes */

#include "list.h"
#include <stdio.h>
#include <stdlib.h>


static ListNode *listNodeCreate() {
    ListNode *node = (ListNode *) malloc(sizeof(ListNode));

    if (node == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    node->data = NULL;
    node->next = NULL;

    return node;
}

List *listCreate() {
    List *list = (List *) malloc(sizeof(List));

    if (list == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    list->head = NULL;
    list->size = 0;

    return list;
}


/* destroys list and all hnodes in it */
void listDestroy(List *list) {
    ListNode *node;
    ListNode *temp;

    if (list == NULL) {
        return;
    }

    node = list->head;
    while (node != NULL) {
        temp = node;
        htreeDestroy(temp->data);
        node = node->next;
        free(temp);
    }

    free(list);
}


/* inserts huffman node into sorted linked list, in ascending order
 * of frequency. if there's a tie, SMALLEST CHAR comes first */
List *listInsert(List *list, HNode *data) {
    ListNode *cur;
    ListNode *prev;

    ListNode *node = listNodeCreate();
    node->data = data;

    /* if list is NULL, create list and insert data */
    if (list == NULL) {
        list = listCreate();
        list->head = node;
        list->size++;
        return list;
    }

    if (list->head == NULL) {
        list->head = node;
        list->size++;
        return list;
    }

    cur = list->head;
    prev = NULL;
    while (cur != NULL) {
        if (hnodeCompare(cur->data, node->data) >= 0) {
            /* if (cur->data->freq >= node->data->freq) { */
            /* modify head if inserting to the beginning */
            if (prev == NULL) {
                list->head = node;
                node->next = cur;
                list->size++;
                return list;
            }

            /* otherwise, insert like this */
            prev->next = node;
            node->next = cur;
            list->size++;
            return list;
        }
        prev = cur;
        cur = cur->next;
    }

    prev->next = node;  /* add to the end */
    list->size++;
    return list;
}


/* inserts huffman node into sorted linked list, in ascending order
 * of frequency. if there's a tie, NEWEST comes first */
List *listInsert2(List *list, HNode *data) {
    ListNode *cur;
    ListNode *prev;

    ListNode *node = listNodeCreate();
    node->data = data;

    /* if list is NULL, create list and insert data */
    if (list == NULL) {
        list = listCreate();
        list->head = node;
        list->size++;
        return list;
    }

    if (list->head == NULL) {
        list->head = node;
        list->size++;
        return list;
    }

    cur = list->head;
    prev = NULL;
    while (cur != NULL) {
        if (cur->data->freq >= node->data->freq) {
            /* modify head if inserting to the beginning */
            if (prev == NULL) {
                list->head = node;
                node->next = cur;
                list->size++;
                return list;
            }

            /* otherwise, insert like this */
            prev->next = node;
            node->next = cur;
            list->size++;
            return list;
        }
        prev = cur;
        cur = cur->next;
    }

    prev->next = node;  /* add to the end */
    list->size++;
    return list;
}


HNode *listRemoveHead(List *list) {
    ListNode *listnode;
    HNode *hnode;

    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    /* remove head ListNode, return HNode data that was removed */
    listnode = list->head;
    hnode = list->head->data;
    list->head = list->head->next;
    free(listnode);
    list->size--;
    return hnode;
}

