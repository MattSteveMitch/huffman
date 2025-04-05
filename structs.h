#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdlib.h>
#include <stdio.h>

// Here's my "redneck valgrind" imitation since I can't get valgrind to work
int mallocs = 0;

void* Malloc(int bytes) {
    mallocs++;
    return malloc(bytes);
}

void Free(void* segment) {
    if (segment != NULL) {
        mallocs--;
    }
    free(segment);
}

void Exit(int exit_code) {
    printf("Unfreed memory pointers: %i", mallocs);
    exit(exit_code);
}
// Capitalize Malloc, Free, and Exit throughout your program and it 
// should tell you if there's a memory leak upon Exit

struct string {
    int length;
    char* val;
};

struct char_and_count {
    unsigned char val;
    unsigned int count;
};

struct charTreeNode {
    unsigned char ch;
    struct charTreeNode* left_child;
    struct charTreeNode* right_child;
    unsigned int count;
    unsigned char num_of_children;
};

struct decodingNode {
    struct decodingNode* left_child;
    struct decodingNode* right_child;
    short val;
};

void deleteEncodeTree(struct charTreeNode* root) {
    if (root == NULL) {
        return;
    }
    deleteEncodeTree(root->left_child);
    deleteEncodeTree(root->right_child);

    free(root);
}

void deleteDecodeTree(struct decodingNode* root) {
    if (root == NULL) {
        return;
    }
    deleteDecodeTree(root->left_child);
    deleteDecodeTree(root->right_child);

    free(root);
}

void shiftToBegin(struct string* buffer, int startInd) {
    int num_of_chars = buffer->length - startInd;
    for (int i = 0; i <= num_of_chars; i++) {
        buffer->val[i] = buffer->val[i + startInd];
    }
    buffer->length -= startInd;
}

char CstringLen(char* str) {
    char ind = 0;
    while (str[ind] != 0) {
        ind++;
    }
    return ind;
}

struct char_and_count* newCC() {
    struct char_and_count* ret_val = malloc(sizeof(struct char_and_count));
    return ret_val;
}

struct decodingNode* newDecodeNode(struct decodingNode* left_child,
    struct decodingNode* right_child, short val) {
        struct decodingNode* ret_val = malloc(sizeof(struct decodingNode));
        ret_val->left_child = left_child;
        ret_val->right_child = right_child;
        ret_val->val = val;
        return ret_val;
}

struct charTreeNode* newTreeNode(struct char_and_count* leaf, struct charTreeNode* left_child, 
                             struct charTreeNode* right_child) {

    struct charTreeNode* ret_val = malloc(sizeof(struct charTreeNode));
    if (leaf != NULL) {
        ret_val->ch = leaf->val;
        ret_val->count = leaf->count;
    }
    ret_val->left_child = left_child;
    ret_val->right_child = right_child;

    int num_children = 0;

    if (left_child != NULL) {
        num_children += 1 + left_child->num_of_children;
    }
    if (right_child != NULL) {
        num_children += 1 + right_child->num_of_children;
    }

    ret_val->num_of_children = num_children;

    return ret_val;
}

struct string* convert_stringEasy(char* val) {
    struct string* ret_val = malloc(sizeof(struct string));
    ret_val->val = val;
    int length = 0;
    for (int i = 0; val[i] != 0; i++) {
        length++;
    }
    ret_val->length = length;

    return ret_val;
}

void append(struct string* str, struct string* appendage, char discardAppendage) {
    int index = str->length;
    for (int i = 0; i <= appendage->length; i++) {
        str->val[index] = appendage->val[i];
        index++;
    }
    
    str->length += appendage->length;
    if (discardAppendage == 2) {
        free(appendage->val);
    }
    if (discardAppendage) {
        free(appendage);
    }
}

char* copyString(char* pattern, int length) {
    char* copy = malloc(length + 1);
    int i;
    for (i = 0; pattern[i] != 0 && i < length; i++) {
        copy[i] = pattern[i];
    }
    copy[i] = 0;

    return copy;
}

char* char_as_binary_str(unsigned char c) {
    char* ret_val = malloc(9);
    ret_val[8] = 0;
    unsigned char c2;
    unsigned char digit;
    for (char i = 7; i >= 0; i--) {
        c2 = c;
        c = c >> 1;

        digit = c2 - (c << 1) + 48;
        ret_val[i] = digit;
    }

    return ret_val;
}

struct string* convert_string(char* val, int length) {
    struct string* ret_val = malloc(sizeof(struct string));
    ret_val->val = val;
    ret_val->length = length;

    return ret_val;
}


void fill_w_binary_str(unsigned char c, char* dest) {
    dest[8] = 0;
    unsigned char c2;
    unsigned char digit;
    for (char i = 7; i >= 0; i--) {
        c2 = c;
        c = c >> 1;

        digit = c2 - (c << 1) + 48;
        dest[i] = digit;
    }
}

struct string* compressIntoBits(char* binaryString) {
    char* compressed = malloc(34);
    struct string* returnVal = malloc(sizeof(struct string));
    unsigned char shiftDist = 0;
    unsigned char arrayInd = 0;
    unsigned char sum = 0;
    char last = 0;

    for (short i = 0; i < 256; i++) {
        if (shiftDist == 8) {
            shiftDist = 0;
            compressed[arrayInd] = sum;
            arrayInd++;
            sum = 0;
        }
        if (binaryString[i] == '1') {
            sum += 1 << (7 - shiftDist);
        }
        else if (binaryString[i] == '0') {
        }
        else if (binaryString[i] == 0) {
            last = 1;
            break;
        }
        shiftDist++;
    }
    compressed[arrayInd] = sum;
    arrayInd++;
    if (last) {
        compressed[arrayInd] = 8 - shiftDist;
        arrayInd++;
    }
    compressed[arrayInd] = 0;
    returnVal->val = compressed;
    returnVal->length = arrayInd;

    return returnVal;
}

unsigned char compressIntoChar(char* binaryString) {
    unsigned char shiftDist = 0;
    unsigned char sum = 0;

    for (short i = 0; i < 8; i++) {
        if (binaryString[i] == '1') {
            sum += 1 << (7 - i);
        }
        shiftDist++;
    }

    return sum;
}

void swapCC(struct char_and_count** array, int ind1, int ind2) {
    struct char_and_count* placeholder = array[ind1];
    array[ind1] = array[ind2];
    array[ind2] = placeholder;
}

void swapInt(int* array, int ind1, int ind2) {
    int placeholder = array[ind1];
    array[ind1] = array[ind2];
    array[ind2] = placeholder;
}

struct heapNode {
    struct charTreeNode* val;
    int id;
};

struct heap {
    struct heapNode** array;
    int size;
    int capac;
    int totalAdded;
    int* indices;
};

void swapHeapNode(struct heapNode** array, int ind1, int ind2) {
    struct heapNode* placeholder = array[ind1];
    array[ind1] = array[ind2];
    array[ind2] = placeholder;
}

int getLine(char* dest) {
    unsigned char nextInd = 0;
    for (char nextChar = getc(stdin); nextChar != '\n'; nextInd++) {
        dest[nextInd] = nextChar;
        nextChar = getc(stdin);
    }
    dest[nextInd] = 0;

    return nextInd;
}

unsigned int fillString(FILE* in_file, struct string* str, int capac) {
    char lineC[513];
    lineC[0] = 0;
    struct string* line = convert_stringEasy(lineC);
    int charsRead;

    while (str->length <= capac) {
        charsRead = fread(line->val, 1, 512, in_file);

        line->val[charsRead] = 0;
        line->length = charsRead;

        append(str, line, 0);

        if (charsRead != 512) {
            break;
        }
    }
    free(line);

    return charsRead;
}

struct heapNode* newHeapNode(struct charTreeNode* val, int id) {
    struct heapNode* ret_val = malloc(sizeof(struct heapNode));
    ret_val->val = val;
    ret_val->id = id;
    return ret_val;
}

struct heapNode* copyHeapNode(struct heapNode* pattern) {
    struct heapNode* ret_val = malloc(sizeof(struct heapNode));
    ret_val->val = pattern->val;
    ret_val->id = pattern->id;
    return ret_val;
}

int findParentInd(int childInd) {
    return (childInd + 1) / 2 - 1;
}

int findChildInd(char right, int parentInd) {
    if (right) { // convert to boolean
        right = 1;
    }
    return (parentInd + 1) * 2 + right - 1;
}

char isLess(struct charTreeNode* val1, struct charTreeNode* val2) {
    return val1->count < val2->count ||
           (val1->count == val2->count && 
            val1->num_of_children < val2->num_of_children);
}

void percolateUp(struct heap* self, int startInd) {
    struct heapNode* child = self->array[startInd];
    int parentInd = findParentInd(startInd);
    if (parentInd < 0) {
        return;
    }

    struct heapNode* parent = self->array[parentInd];

    if (isLess(child->val, parent->val)) {

        swapHeapNode(self->array, parentInd, startInd);
        swapInt(self->indices, parent->id, child->id);
        percolateUp(self, parentInd);
    }
}


struct heap* newHeap(struct heapNode** vals_in_order, int arrayLen, int max_capac) {
    struct heap* self = malloc(sizeof(struct heap));

    self->array = malloc(sizeof(struct heapNode*) * max_capac);

    for (int i = 0; i < arrayLen; i++) {
        self->array[i] = vals_in_order[i];
    }
    
    self->size = arrayLen;
    self->totalAdded = arrayLen;
    self->capac = max_capac;

    self->indices = malloc(sizeof(int) * max_capac);
    for (int i = 0; i < arrayLen; i++) {
        self->indices[i] = i;
    }

    for (int i = 0; i < arrayLen; i++) {
        percolateUp(self, i);
    }

    return self;
}

void percolateDown(struct heap* self, int startInd, char all_the_way) {
    // all_the_way indicates whether we're percolating down because we just deleted a node and
    // need to fill the gap; if not, we've decreased a node's priority and want to bring it
    // down to its proper place.
    int arrayLen = self->size;
    int right_child_ind = findChildInd(1, startInd);
    struct heapNode* parent = self->array[startInd];

    int num_of_children;
    int left_child_ind;
    struct heapNode* left_child;

    if (right_child_ind == arrayLen) {
        num_of_children = 1;
        left_child_ind = right_child_ind - 1;
        left_child = self->array[left_child_ind];
    }
    else if (right_child_ind < arrayLen) {
        num_of_children = 2;
    }
    else {
        num_of_children = 0;
    }

    if (num_of_children == 0) {
        if (all_the_way) {
            if (startInd != arrayLen - 1) {
                struct heapNode* lastNode = self->array[arrayLen - 1];
                self->array[startInd] = lastNode;
                self->indices[lastNode->id] = startInd;
                percolateUp(self, startInd);
            }
            self->size--;
        }
        return;
    }

    else if (num_of_children == 1) {
        if (all_the_way || isLess(left_child->val, parent->val)) {
            self->array[startInd] = left_child;
            self->indices[left_child->id] = startInd;

            if (!all_the_way) {  // In this case we want to swap the parent and child,
                self->array[left_child_ind] = parent;  // not just bring the child up
                self->indices[parent->id] = left_child_ind;
            }
        }

        if (all_the_way) {
            self->size--;
        }

        return;
    }

    else {
        left_child_ind = right_child_ind - 1;
        struct heapNode* child1 = self->array[left_child_ind];
        struct heapNode* child2 = self->array[right_child_ind];

        if (isLess(child2->val, child1->val)) {
            if (all_the_way || isLess(child2->val, parent->val)) {
                self->array[startInd] = child2;
                self->indices[child2->id] = startInd;

                if (!all_the_way) {  // In this case we want to swap the parent and child,
                    self->array[right_child_ind] = parent;  // not just bring the child up
                    self->indices[parent->id] = right_child_ind;
                }
                percolateDown(self, right_child_ind, all_the_way);
            }
        }

        else {
            if (all_the_way || isLess(child1->val, parent->val)) {
                self->array[startInd] = child1;
                self->indices[child1->id] = startInd;

                if (!all_the_way) {  // In this case we want to swap the parent and child,
                    self->array[left_child_ind] = parent;  // not just bring the child up
                    self->indices[parent->id] = left_child_ind;
                }
                percolateDown(self, left_child_ind, all_the_way);
            }
        }
    }
}

int add(struct heap* self, struct charTreeNode* newNode) {
    int id = self->totalAdded;
    int spot = self->size;
    if (spot == self->capac) {
        return 0;
    }

    struct heapNode* node = newHeapNode(newNode, id);
    self->array[spot] = node;
    self->size++;
    self->indices[id] = id;
    self->totalAdded++;
    percolateUp(self, spot);

    return 1;
}

struct heapNode* pop(struct heap* self) {
    if (self->size == 0) {
        return NULL;
    }
    struct heapNode* top = self->array[0];
    self->indices[top->id] = -1;
    percolateDown(self, 0, 1);
    return top;
}

int changeKey(struct heap* self, int node_id, struct charTreeNode* newKey) {
    int index = self->indices[node_id];
    if (index == -1) {
        return 0;
    }

    struct heapNode* node = self->array[index];
    struct charTreeNode* oldKey = node->val;
    node->val = newKey;

    if (isLess(newKey, oldKey)) {
        percolateUp(self, index);
    }
    else {
        percolateDown(self, index, 0);
    }
    return 1;
}

struct heapNode* getNode(struct heap* self, int id) {
    int index = self->indices[id];
    if (index == -1) {
        return NULL;
    }
    return self->array[index];
}

#endif
