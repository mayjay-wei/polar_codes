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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree_encode.h"

#pragma GCC optimize("O3")

#define POW2(x) (1 << (x))

static Node *g_root = NULL;

/**
 * @brief 建立一個新的節點
 * @param parent 父節點指標
 * @param depth 節點深度
 * @param data_length 資料長度
 * @return Node* 新建立的節點指標
 */
Node *create_node(Node *parent, size_t depth, size_t data_length) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) return NULL;  // 記憶體分配失敗

    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->depth = depth;
    node->data_length = data_length;

    // 分配資料記憶體
    node->data = (uint8_t *)calloc(data_length, sizeof(uint8_t));
    if (!node->data) {
        free(node);
        return NULL;  // 記憶體分配失敗
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
static Node *build_tree_recursive(Node *parent, size_t current_depth,
                                  size_t max_depth) {
    // 計算當前層級的資料長度：葉節點長度為1，向上每層乘以2
    size_t data_length = 1;
    for (size_t i = 0; i < (max_depth - current_depth); i++) {
        data_length *= 2;
    }

    Node *node = create_node(parent, current_depth, data_length);
    if (!node) return NULL;

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
void free_tree(Node *node) {
    if (!node) return;

    // 遞迴釋放子節點
    free_tree(node->left);
    free_tree(node->right);

    // 釋放當前節點的資料和節點本身
    free(node->data);
    free(node);
}

// To pre-allocate the size to the tree since the depth is known for certain
// Polar encoding.
void creatTree(const size_t stage) {
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
Node *get_root() { return g_root; }

/**
 * @brief 取得樹在指定深度的節點數量
 * @param depth 深度
 * @return size_t 該深度的節點數量
 */
size_t get_nodes_at_depth(size_t depth) {
    return POW2(depth);  // 2^depth
}

/**
 * @brief 印出節點資訊（除錯用）
 * @param node 要印出的節點
 */
void print_node_info(const Node *node) {
    if (!node) {
        printf("Node: NULL\n");
        return;
    }

    printf("Node at depth %zu, data_length: %zu\n",
           node->depth,
           node->data_length);
    printf("Has left child: %s, Has right child: %s\n",
           node->left ? "Yes" : "No",
           node->right ? "Yes" : "No");
}

/**
 * @brief 檢查樹是否為葉節點
 * @param node 要檢查的節點
 * @return bool true 如果是葉節點
 */
bool is_leaf(const Node *node) { return node && !node->left && !node->right; }

/**
 * @brief 收集所有葉節點（由左到右順序）
 * @param node 當前節點
 * @param leaves 葉節點陣列
 * @param index 當前索引指標
 */
void collect_leaves(Node *node, Node **leaves, size_t *index) {
    if (!node) return;

    // 如果是葉節點，加入陣列
    if (is_leaf(node)) {
        leaves[*index] = node;
        (*index)++;
        return;
    }

    // 遞迴遍歷左子樹和右子樹（保持由左到右順序）
    collect_leaves(node->left, leaves, index);
    collect_leaves(node->right, leaves, index);
}

/**
 * @brief 將輸入位元設置到葉節點
 * @param input_bits 輸入位元陣列
 * @param code_length 編碼長度
 */
void set_input_to_leaves(uint8_t *input_bits, size_t code_length) {
    if (!g_root || !input_bits) return;

    // 分配葉節點陣列記憶體, with VLA
    Node *leaves[code_length];
    // 初始化陣列為 NULL
    for (size_t i = 0; i < code_length; i++) {
        leaves[i] = NULL;
    }

    // 收集所有葉節點
    size_t index = 0;
    collect_leaves(g_root, leaves, &index);

    // 檢查收集到的葉節點數量是否正確
    if (index != code_length) return;  // 葉節點數量不匹配

    // 設置輸入資料到葉節點
    for (size_t i = 0; i < code_length; i++) {
        if (leaves[i] && leaves[i]->data) {
            leaves[i]->data[0] = input_bits[i];
        }
    }
}

/**
 * @brief 遞迴執行極化編碼（深度優先，後序遍歷）
 * @param node 當前節點
 *
 * 編碼原理：
 * - 使用後序遍歷確保子節點先處理完成
 * - 每個內部節點執行極化編碼的蝶形運算
 * - 左半部分：left[i] ⊕ right[i]（對應原始算法的XOR運算）
 * - 右半部分：right[i]（直接複製）
 */
void encode_recursive(Node *node) {
    if (!node) return;

    // 葉節點不需要計算，直接返回
    if (is_leaf(node)) return;

    // 後序遍歷：先處理子節點
    encode_recursive(node->left);
    encode_recursive(node->right);

    // 執行極化編碼的蝶形運算
    // 每個內部節點的資料長度是子節點的兩倍
    size_t half_length = node->data_length / 2;

    // 驗證子節點的資料長度
    if (!node->left || !node->right || node->left->data_length != half_length
        || node->right->data_length != half_length) {
        return;  // 資料結構不一致
    }

    // 蝶形運算：實現極化編碼的核心邏輯
    for (size_t i = 0; i < half_length; i++) {
        // 上半部分：左子節點 XOR 右子節點
        node->data[i] = node->left->data[i] ^ node->right->data[i];
        // 下半部分：直接複製右子節點
        node->data[i + half_length] = node->right->data[i];
    }
}

/**
 * @brief 從根節點提取編碼結果到輸出陣列
 * @param output_codeword 輸出碼字陣列
 * @param code_length 編碼長度
 */
void extract_encoded_result(uint8_t *output_codeword, size_t code_length) {
    if (!g_root || !output_codeword || !g_root->data) return;

    // 檢查根節點的資料長度是否匹配
    if (g_root->data_length != code_length) return;

    // 從根節點複製編碼結果
    memcpy(output_codeword, g_root->data, code_length * sizeof(uint8_t));
}

/**
 * @brief 主要樹狀編碼介面函數
 * @param input_codeword 輸入/輸出碼字陣列
 * @param code_length 編碼長度
 *
 * 完整的編碼流程：
 * 1. 驗證樹是否已初始化
 * 2. 設置輸入資料到葉節點
 * 3. 執行深度優先遞迴編碼
 * 4. 提取編碼結果（in-place 修改）
 */
void tree_encode(uint8_t *input_codeword, size_t code_length) {
    if (!g_root || !input_codeword) {
        printf("Error: Tree not initialized or invalid input\n");
        return;
    }

    // 驗證樹的結構是否與編碼長度匹配
    if (g_root->data_length != code_length) {
        printf("Error: Tree structure mismatch (expected %zu, got %zu)\n",
               code_length,
               g_root->data_length);
        return;
    }

    // 步驟1：設置輸入資料到葉節點
    set_input_to_leaves(input_codeword, code_length);

    // 步驟2：執行樹狀編碼（深度優先後序遍歷）
    encode_recursive(g_root);

    // 步驟3：提取編碼結果（直接修改原陣列）
    extract_encoded_result(input_codeword, code_length);
}

/**
 * @brief 印出樹的結構（除錯用）
 * @param node 當前節點
 * @param indent 縮進層級
 */
void print_tree_structure(Node *node, int indent) {
    if (!node) return;

    // 印出當前節點資訊
    for (int i = 0; i < indent; i++)
        printf("  ");
    printf("Depth %zu, Length %zu", node->depth, node->data_length);

    if (is_leaf(node)) {
        printf(" [LEAF: %u]\n", node->data[0]);
    } else {
        printf(" [INTERNAL]\n");
        // 遞迴印出子樹
        print_tree_structure(node->left, indent + 1);
        print_tree_structure(node->right, indent + 1);
    }
}
