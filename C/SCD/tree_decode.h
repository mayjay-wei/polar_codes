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
 * @brief Tree-based SC decoder (low-level API)
 * @param channel_llr Channel LLR values (float array)
 * @param frozen_bits Frozen bit indicators
 * @param decoded_bits Output decoded bits
 * @param code_length Code length
 */
void treeScDecode(const float *channel_llr, bool *frozen_bits,
                    uint8_t *decoded_bits, size_t code_length);

/**
 * @brief Tree-based decoder with float LLR (optimized for accuracy)
 * @param msg_cap Output information bits
 * @param K Number of information bits
 * @param LLR_float Float LLR input
 * @param info_nodes Information bit indicators
 * @param data_pos Information bit positions
 */
void treeDecode(int *msg_cap, const unsigned K, const float *LLR_float,
                     const bool *info_nodes, const int *data_pos);

#endif // TREE_DECODE_H