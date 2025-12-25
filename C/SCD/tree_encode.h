#ifndef TREE_ENCODE_H
#define TREE_ENCODE_H

#include <stdint.h>
#include <stddef.h>

typedef struct Node {
    struct Node *parent;
    struct Node *left;
    struct Node *right;
    uint8_t *data;
    size_t depth;
    size_t data_length;
} Node;

void creatTree(const size_t stage);
void tree_encode(uint8_t *input_codeword, size_t code_length);
Node *get_root();
void free_tree(Node *node);

#endif