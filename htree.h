typedef struct HNode {
    char chr;
    int freq;
    struct HNode *left;
    struct HNode *right;
} HNode;


int createCodes(HNode *htree, char **codes, char *path, int len);

int hnodeCompare(HNode *node1, HNode *node2);

HNode *htreeCreate(int freq, char chr);

void htreeDestroy(HNode *htree);

