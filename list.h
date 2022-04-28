#include "htree.h"

typedef struct ListNode {
    HNode *data;
    struct ListNode *next;
} ListNode;

typedef struct List {
    ListNode *head;
    int size;
} List;


List *listCreate();

void listDestroy(List *list);

List *listInsert(List *list, HNode *data);

List *listInsert2(List *list, HNode *data);

HNode *listRemoveHead(List *list);

