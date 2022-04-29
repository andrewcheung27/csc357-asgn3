#include "list.h"  /* this also includes htree.h */


List *constructHTree(unsigned int *freqTable, int size);

void writeBuf(char c, char *buf, unsigned int *size, unsigned int *capacity);
