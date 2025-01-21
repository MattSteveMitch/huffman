#include <stdio.h>
#include <string.h>
#include "structs.h"


void DFS(struct charTreeNode* tree, char* pathArray, int pathArrayInd, char** encoding) {
    if (tree->left_child != NULL) {
        pathArray[pathArrayInd] = '0';
        pathArrayInd++;
        DFS(tree->left_child, pathArray, pathArrayInd, encoding);

        pathArrayInd--;
    }
    else {
        pathArray[pathArrayInd] = 0;
        encoding[tree->ch] = copyString(pathArray, pathArrayInd);
        return;
    }
    
    pathArray[pathArrayInd] = '1';
    pathArrayInd++;
    DFS(tree->right_child, pathArray, pathArrayInd, encoding);

    pathArrayInd--;
    return;
}

void makeNewFileName(struct string* old_name) {
    unsigned char name_ind = old_name->length - 1;
    struct string* suffix = convert_stringEasy(".huf");

    while (old_name->val[name_ind] != '.') {
        name_ind--;
    }

    old_name->val[name_ind] = 0;
    old_name->length = name_ind;
    append(old_name, suffix, 1);
}

char stringLen(char* str) {
    char ind = 0;
    while (str[ind] != 0) {
        ind++;
    }
    return ind;
}


int main() {
    FILE* in_file;
    char* filename = malloc(sizeof(char) * 140);
    char* contentsC = malloc(sizeof(char) * 2049);
    contentsC[0] = 0;
    struct string* contents = convert_stringEasy(contentsC);

    printf("Enter file name: ");
    getLine(filename);

    in_file = fopen(filename, "rb");
    if (in_file == NULL) {
        printf("Couldn't open file\n");
        return 1;
    }
    
    fillString(in_file, contents, 1536);

    struct char_and_count* charCount = malloc(sizeof(struct char_and_count) * 256);

    for (short i = 0; i < 256; i++) {
        charCount[i].val = i;
        charCount[i].count = 0;
    }

    for (int i = 0; i < contents->length; i++) {
        unsigned char c = contents->val[i];
        charCount[c].count++;
        if (i == 1535) {
            shiftToBegin(contents, 1536);
            fillString(in_file, contents, 1536);
            i = -1;
        }
    }
    
    fclose(in_file);

    struct heapNode** tree_nodes = malloc(sizeof(struct heapNode*) * 256);

    for (short c = 0; c < 256; c++) {
        struct charTreeNode* treeNode = newTreeNode(&charCount[c], NULL, NULL);
        tree_nodes[c] = newHeapNode(treeNode, c);
    }

    struct heap* h = newHeap(tree_nodes, 256, 511);
    free(tree_nodes);

    struct char_and_count* countVal = newCC();
    countVal->val = 0;

    while (h->size > 1) {
        struct heapNode* next1 = pop(h);
        struct heapNode* next2 = pop(h);

        countVal->count = next1->val->count + next2->val->count;
        struct charTreeNode* charNode1 = newTreeNode(countVal, next1->val, next2->val);

        free(next1);
        free(next2);

        add(h, charNode1);
    }
    free(countVal);

    struct heapNode* treePackage = pop(h);
    struct charTreeNode* tree = treePackage->val;
    free(treePackage);
    if (h->size != 0) {
        printf("Error; priority queue not empty\n");
        exit(1);
    }

    free(h->array);
    free(h->indices);
    free(h);

    char* pathArray = malloc(sizeof(char) * 257);
    char** encodingMap = malloc(sizeof(char*) * 256);

    DFS(tree, pathArray, 0, encodingMap);

    deleteEncodeTree(tree);
    free(pathArray);
    
    char* bin_bufferC = malloc(513);
    bin_bufferC[0] = 0;
    struct string* bin_buffer = convert_stringEasy(bin_bufferC);
    
    int bookmark = 0;
    char* compressedC = malloc(514);
    compressedC[0] = 0;
    struct string* compressed = convert_stringEasy(compressedC);

    struct string* out_file_name = convert_stringEasy(copyString(filename, stringLen(filename) + 15));
    makeNewFileName(out_file_name);

    FILE* out_file = fopen(out_file_name->val, "wb");

    free(out_file_name->val);
    free(out_file_name);

    unsigned char encodingLen;
    short ind;
    
    for (ind = 0; ind < 256; ind++) {
        if (charCount[ind].count > 0) {
            encodingLen = stringLen(encodingMap[ind]);
            append(bin_buffer, convert_string(char_as_binary_str(ind), 8), 2);
            append(bin_buffer, convert_string(char_as_binary_str(encodingLen), 8), 2);
            append(bin_buffer, convert_string(encodingMap[ind], encodingLen), 1);

            if (bin_buffer->length > 255) {
                append(compressed, compressIntoBits(bin_buffer->val), 2);

                shiftToBegin(bin_buffer, 256);

                if (compressed->length > 480) {
                    fwrite(compressed->val, 1, compressed->length, out_file);
                    compressed->length = 0;
                    compressed->val[0] = 0;
                }
            }
        }
    }

    append(bin_buffer, convert_string(char_as_binary_str(0), 8), 2);
    short n = fwrite(compressed->val, 1, compressed->length, out_file);

    compressed->length = 0;
    compressed->val[0] = 0;

    free(charCount);

    in_file = fopen(filename, "rb");
    if (in_file == NULL) {
        printf("Couldn't open file\n");
        return 1;
    }
    free(filename);

    contents->val[0] = 0;
    contents->length = 0;
    fillString(in_file, contents, 1536);

    char flush = 0;
    while (bookmark < contents->length) {
        flush = 0;
        for (; bin_buffer->length < 256; bookmark++) {
            unsigned char nextChar = contents->val[bookmark];
            if (bookmark == contents->length) {
                break;
            }
            char* appendage = encodingMap[nextChar];
            for (int j = 0; appendage[j] != 0; j++) {
                bin_buffer->val[bin_buffer->length] = appendage[j];
                bin_buffer->length++;
            }
            if (bookmark == 1535) {
                shiftToBegin(contents, 1536);
                fillString(in_file, contents, 1536);
                bookmark = -1;
            }
        }

        bin_buffer->val[bin_buffer->length] = 0;
        append(compressed, compressIntoBits(bin_buffer->val), 2);

        bin_buffer->val[0] = 0;
        if (bin_buffer->length >= 256) {
            shiftToBegin(bin_buffer, 256);
            flush = 1;
        }

        if (compressed->length >= 512) {
            n = fwrite(compressed->val, 1, compressed->length, out_file);
            compressed->length = 0;
            compressed->val[0] = 0;
        }
    }
    if (flush) {
        append(compressed, compressIntoBits(bin_buffer->val), 2);
    }
    fwrite(compressed->val, 1, compressed->length, out_file);

    fclose(out_file);
    fclose(in_file);

    for (int i = 0; i < 256; i++) {
        free(encodingMap[i]);
    }
    free(encodingMap);
    free(bin_buffer->val);
    free(bin_buffer);
    free(compressed->val);
    free(compressed);
    free(contents->val);
    free(contents);

    return 0;
}
