#include "structs.h"
#include <stdio.h>

struct decodingNode* buildTree(char** key) {
    struct decodingNode* root = newDecodeNode(NULL, NULL, -1);
    char* charString;
    struct decodingNode* currNode;
    unsigned char currChar;
    struct decodingNode* child;

    for (short i = 0; i < 256; i++) {
        charString = key[i];
        currNode = root;
        if (charString == NULL) {
            continue;
        }

        for (int j = 0; charString[j] != 0; j++) {
            currChar = charString[j];

            if (currChar == '0') {
                child = currNode->left_child;
                if (child == NULL) {
                    currNode->left_child = newDecodeNode(NULL, NULL, -1);
                }
                currNode = currNode->left_child;
            }
            else if (currChar == '1') {
                child = currNode->right_child;
                if (child == NULL) {
                    currNode->right_child = newDecodeNode(NULL, NULL, -1);
                }
                currNode = currNode->right_child;
            }
            else {
                printf("Invalid string\n");
                exit(1);
            }
        }
        currNode->val = i;
    }

    return root;
}

void finish(struct string* decompressed, int currSpot, struct string* out_buffer,
            struct decodingNode* tree) {
    
    char* bitsToTrim = decompressed->val + currSpot - 8;
    char excessBits = compressIntoChar(bitsToTrim);
    int beginInd = decompressed->length - 8 - excessBits;
    int scanInd = beginInd;
    struct decodingNode* currNode;

    char charsToErase = -1;
    char nextDigit = 1;
    while (nextDigit) {
        charsToErase++;
        currNode = tree;
        for (; currNode->val == -1; scanInd++) {
            nextDigit = decompressed->val[scanInd];
            if (nextDigit == '0') {
                currNode = currNode->left_child;
            }
            else if (nextDigit == '1') {
                currNode = currNode->right_child;
            }
            else {
                break;
            }
        }
    }

    out_buffer->length -= charsToErase;
    out_buffer->val[out_buffer->length] = 0;

    deleteDecodeTree(tree);
}

void releaseVals(void* ptr1, void* ptr2, void* ptr3, void* ptr4, void* ptr5, void* ptr6) {
    free(ptr1);
    free(ptr2);
    free(ptr3);
    free(ptr4);
    free(ptr5);
    free(ptr6);
}

char getSuffixInd(struct string* fileName) {
    unsigned char dotInd = fileName->length - 1;
    char* pattern = "_compressed.txt";

    while (fileName->val[dotInd] != '.') {
        dotInd--;
    }
    return dotInd;
}

int main() {
    char* out = malloc(273);
    struct string* out_buffer = convert_string(out, 0);

    char* filenameC = malloc(sizeof(char) * 140);
    
    printf("Enter file name: ");
    int filenameLen = getLine(filenameC);

    struct string* filename = convert_stringEasy(filenameC);

    char* out_fileC = copyString(filename->val, filename->length + 17);
    struct string* out_file_name = convert_string(out_fileC, filename->length);

    char suffixInd = getSuffixInd(filename);
    
    out_file_name->val[suffixInd] = 0;
    out_file_name->length = suffixInd;

    struct string* appendage = convert_string("_decompressed.txt", 17);

    append(out_file_name, appendage, 1);
    
    FILE* in_file = fopen(filename->val, "rb");
    if (in_file == NULL) {
        printf("Couldn't open file\n");
        return 1;
    }

    free(filename->val);
    free(filename);
    
    char** key = malloc(sizeof(char*) * 256);
    for (int i = 0; i < 256; i++) {
        key[i] = NULL;
    }

    char* rawBufferC = malloc(65);
    int bufferBM = 0;
    char* decompressedC = malloc(513);
    decompressedC[0] = 0;
    unsigned char nextChar;
    unsigned char nextLen;

    int numberRead = fread(rawBufferC, 1, 64, in_file);
    struct string* rawBuffer = convert_string(rawBufferC, numberRead);
    struct string* decompressed = convert_string(decompressedC, 0);

    unsigned char max_nulls = 1;
    if (rawBuffer->val[0] == 0) {
        max_nulls = 2;
    }

    unsigned char nulls = 0;
    int decomprBM = 0;

    for (decomprBM = 0; decomprBM < 512; decomprBM += 8) {
        fill_w_binary_str(rawBuffer->val[bufferBM], decompressed->val + decomprBM);
        bufferBM++;
        decompressed->length += 8;
    }
    
    decomprBM = 0;
    bufferBM = 0;
    short bytesToRead;

    while (1) {
        nextChar = compressIntoChar(decompressed->val + decomprBM);
        decomprBM += 8;
        if (nextChar == 0) {
            nulls++;
            if (nulls == max_nulls) {
                break;
            }
        }

        nextLen = compressIntoChar(decompressed->val + decomprBM);
        decomprBM += 8;

        if (decomprBM + nextLen + 16 > 504) {
            shiftToBegin(decompressed, decomprBM);

            bytesToRead = (512 - decompressed->length) / 8;
            rawBuffer->length = fread(rawBuffer->val, 1, bytesToRead, in_file);
            bufferBM = 0;

            for (decomprBM = decompressed->length; bufferBM != rawBuffer->length; decomprBM += 8) {
                fill_w_binary_str(rawBuffer->val[bufferBM], decompressed->val + decomprBM);
                bufferBM++;
                decompressed->length += 8;
            }
            decomprBM = 0;
        }

        key[nextChar] = copyString(decompressed->val + decomprBM, nextLen);
        decomprBM += nextLen;
    }

    struct decodingNode* tree = buildTree(key);

    for (int i = 0; i < 256; i++) {
        free(key[i]);
    }
    free(key);

    int scanInd;
    struct decodingNode* currNode;
    char nextDigit;
    char overflowing = 0;
    char finishing = 0;
    char finished = 0;
    short bufferLen;

    FILE* out_file = fopen(out_file_name->val, "wb");
    if (out_file == NULL) {
        printf("Couldn't open file\n");
        return 1;
    }

    free(out_file_name->val);
    free(out_file_name);

    while (!finished) {

        currNode = tree;
        for (scanInd = decomprBM; currNode->val == -1; scanInd++) {
            if (decompressed->length - scanInd <= 16 && !finishing) {
                overflowing = 1;
                break;
            }

            nextDigit = decompressed->val[scanInd];
            if (nextDigit == '0') {
                currNode = currNode->left_child;
            }
            else if (nextDigit == '1') {
                currNode = currNode->right_child;
            }
            else {
                fclose(in_file);
                finish(decompressed, scanInd, out_buffer, tree);
                fwrite(out_buffer->val, 1, out_buffer->length, out_file);
                fclose(out_file);
                releaseVals(out_buffer->val, out_buffer, decompressed->val, decompressed,
                            rawBuffer->val, rawBuffer);

                return 0;
            }
        }


        if (!overflowing) {
            nextChar = currNode->val;
            nextLen = scanInd - decomprBM;

            bufferLen = out_buffer->length;
            out_buffer->val[bufferLen] = nextChar;
            out_buffer->length++;
            
            if (bufferLen == 271) {
                fwrite(out_buffer->val, 1, 256, out_file);
                shiftToBegin(out_buffer, 256);
            }
        }

        else {
            if (!finishing) {
                shiftToBegin(decompressed, decomprBM);
            }

            bytesToRead = (512 - decompressed->length) / 8;

            rawBuffer->length = fread(rawBuffer->val, 1, bytesToRead, in_file);

            if (rawBuffer->length == 0) {
                finishing = 1;
            }
            bufferBM = 0;

            for (decomprBM = decompressed->length; bufferBM != rawBuffer->length; decomprBM += 8) {
                fill_w_binary_str(rawBuffer->val[bufferBM], decompressed->val + decomprBM);
                bufferBM++;
                decompressed->length += 8;
            }
            decomprBM = 0;
            overflowing = 0;
            continue;
        }

        decomprBM += nextLen;
    }

}
