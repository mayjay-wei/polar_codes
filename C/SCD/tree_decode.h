#ifndef TREE_DECODE_H
#define TREE_DECODE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @file tree_decode.h
 * @brief Tree-based polar code decoder API
 * @author TC
 * @date 2025-12-26
 */

// Forward declaration of DecodeNode
typedef struct DecodeNode DecodeNode;

/**
 * @brief Create decode tree with specified stage depth
 * @param stage Number of stages (tree depth)
 */
void createDecodeTree(const size_t stage);

/**
 * @brief Get root node of decode tree
 * @return Pointer to root DecodeNode
 */
DecodeNode* getDecodeRoot(void);




/**
 * @brief Tree-based decoder with float LLR (optimized for accuracy)
 * @param msg_cap Output information bits
 * @param K Number of information bits
 * @param LLR_float Float LLR input
 */
void treeDecode(int *msg_cap, const unsigned K, const float *LLR_float);

#endif // TREE_DECODE_H