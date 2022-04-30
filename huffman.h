#include "list.h"  /* this also includes htree.h */


List *constructHTree(unsigned int *freqTable, int size);

void writeBuf(int outfile, char c, char *buf, int *size, int *capacity);
