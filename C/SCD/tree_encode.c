/**
 * @file tree_encode.c
 * @author TC
 * @brief An binary tree structure polar encoding implementation
 * @version 0.1
 * @date 2025-12-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define POW2(x) (1 << (x))

typedef struct Node
{
    struct Node* parent;
    struct Node* left;
    struct Node* right;
    uint8_t *data;
    size_t depth;
    size_t data_length;
}Node;

static Node* g_root = NULL;

/**
 * @brief 建立一個新的節點
 * @param parent 父節點指標
 * @param depth 節點深度
 * @param data_length 資料長度
 * @return Node* 新建立的節點指標
 */
Node* create_node(Node* parent, size_t depth, size_t data_length) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        return NULL; // 記憶體分配失敗
    }
    
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->depth = depth;
    node->data_length = data_length;
    
    // 分配資料記憶體
    node->data = (uint8_t*)calloc(data_length, sizeof(uint8_t));
    if (!node->data) {
        free(node);
        return NULL; // 記憶體分配失敗
    }
    
    return node;
}

/**
 * @brief 遞迴建立樹的函數
 * @param parent 父節點
 * @param current_depth 當前深度
 * @param max_depth 最大深度
 * @return Node* 建立的節點指標
 */
Node* build_tree_recursive(Node* parent, size_t current_depth, size_t max_depth) {
    // 計算當前層級的資料長度：葉節點長度為1，向上每層乘以2
    size_t data_length = 1;
    for (size_t i = 0; i < (max_depth - current_depth); i++) {
        data_length *= 2;
    }
    
    Node* node = create_node(parent, current_depth, data_length);
    if (!node) {
        return NULL;
    }
    
    // 如果還沒到達最大深度，繼續建立子節點
    if (current_depth < max_depth) {
        node->left = build_tree_recursive(node, current_depth + 1, max_depth);
        node->right = build_tree_recursive(node, current_depth + 1, max_depth);
        
        // 檢查子節點建立是否成功
        if (!node->left || !node->right) {
            // 清理已建立的節點
            if (node->left) free(node->left);
            if (node->right) free(node->right);
            free(node->data);
            free(node);
            return NULL;
        }
    }
    
    return node;
}

/**
 * @brief 釋放樹的記憶體
 * @param node 要釋放的節點
 */
void free_tree(Node* node) {
    if (!node) {
        return;
    }
    
    // 遞迴釋放子節點
    free_tree(node->left);
    free_tree(node->right);
    
    // 釋放當前節點的資料和節點本身
    free(node->data);
    free(node);
}

// To pre-allocate the size to the tree since the depth is known for certain
// Polar encoding.
void creatTree(const size_t stage){
    // 首先釋放現有的樹（如果存在）
    if (g_root) {
        free_tree(g_root);
        g_root = NULL;
    }
    
    // 樹的深度 = stage + 1
    size_t tree_depth = stage;  // 深度從0開始計算，所以stage就是最大深度
    
    // 建立新樹
    g_root = build_tree_recursive(NULL, 0, tree_depth);
    
    if (!g_root) {
        // 處理建立失敗的情況
        // 可以在這裡加入錯誤處理邏輯
    }
}

/**
 * @brief 取得根節點指標
 * @return Node* 根節點指標
 */
Node* get_root() {
    return g_root;
}

/**
 * @brief 取得樹在指定深度的節點數量
 * @param depth 深度
 * @return size_t 該深度的節點數量
 */
size_t get_nodes_at_depth(size_t depth) {
    return POW2(depth); // 2^depth
}

/**
 * @brief 印出節點資訊（除錯用）
 * @param node 要印出的節點
 */
void print_node_info(const Node* node) {
    if (!node) {
        printf("Node: NULL\n");
        return;
    }
    
    printf("Node at depth %zu, data_length: %zu\n", node->depth, node->data_length);
    printf("Has left child: %s, Has right child: %s\n", 
           node->left ? "Yes" : "No", 
           node->right ? "Yes" : "No");
}

/**
 * @brief 檢查樹是否為葉節點
 * @param node 要檢查的節點
 * @return bool true 如果是葉節點
 */
bool is_leaf(const Node* node) {
    return node && !node->left && !node->right;
}

