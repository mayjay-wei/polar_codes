#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "functions_SCD.h"

#include <string.h>

#define f_macro(L1, L2) (SIGN(L1) * SIGN(L2) * MIN(ABS(L1), ABS(L2)))
#define g_macro(u, L1, L2) \
    MIN(MAX((((1 - 2 * u) * L1) + L2), -(MAXQR + 1)), MAXQR)
// #define g_macro(u, L1, L2) (((1 - 2*u) * L1) + L2)
#define SIGN(x)   ((x > 0) - (x < 0))
#define ABS(x)    (((x > 0) - (x < 0)) * x)
#define MIN(x, y) ((x < y) ? x : y)
#define MAX(x, y) ((x < y) ? y : x)

/*Uniform distribution number generator*/
bool uniformBinaryRandomNumber() { return rand() % 2; }  // NOLINT(*-msc50-cpp)

/*Gaussian random number generator*/
// Box muller method, see
// https://mathworld.wolfram.com/Box-MullerTransformation.html
float randn(const float mean, const float std) {
    float U1, U2, W;
    static float X1, X2;
    static bool call = false;

    if (call) {
        call = !call;
        return mean + std * X2;
    }

    do {
        U1 = -1 + (float)rand() / RAND_MAX * 2;  // NOLINT(*-msc50-cpp)
        U2 = -1 + (float)rand() / RAND_MAX * 2;  // NOLINT(*-msc50-cpp)
        W = U1 * U1 + U2 * U2;
    } while (W >= 1 || W <= 0);

    const float normalize_coefficient = sqrtf(-2.0f * logf(W) / W);
    X1 = U1 * normalize_coefficient;
    X2 = U2 * normalize_coefficient;

    call = !call;

    return mean + std * X1;
}

/* swap function int data type */
void swap_int(int *a, int *b) {
    const int t = *a;
    *a = *b;
    *b = t;
}

/* partition function int data type */
int partition_int(int arr[], const int low, const int high) {
    const int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap_int(&arr[i], &arr[j]);
        }
    }
    swap_int(&arr[i + 1], &arr[high]);
    return i + 1;
}

/* quicksort function int data type */
void quickSort_int(int arr[], const int low, const int high) {
    if (low < high) {
        const int pi = partition_int(arr, low, high);

        quickSort_int(arr, low, pi - 1);
        quickSort_int(arr, pi + 1, high);
    }
}

/* Non-systematic encoder*/
void Encode(int *codeword) {
    /* Stage after stage */
    for (int stage_idx = 0; stage_idx < POLAR_CODE_STAGE; stage_idx++) {
        /* Group after group in each stage */
        for (int group_idx = 0;
             group_idx < POW2(POLAR_CODE_STAGE - stage_idx - 1);
             group_idx++) {
            // xor_distance of current stage
            const int stride = POW2(stage_idx);
            const int base = POW2(stage_idx + 1) * group_idx;

            /* Subgroup after subgroup */
            for (int i_sg = 0; i_sg < stride; i_sg++) {
                codeword[base + i_sg]
                    = codeword[base + i_sg] ^ codeword[base + i_sg + stride];
            }
        }
    }
}

/* SC Decoder */
void Decode(int *msg_cap, const unsigned K, const int *LLR_Q,
            const bool *info_nodes, const int *data_pos) {
    /* Beliefs */
    int L[POLAR_CODE_STAGE + 1][POLAR_CODE_LENGTH];
    /* Belief initialisation */
    memcpy(L[0], LLR_Q, sizeof(int) * POLAR_CODE_LENGTH);

    /* Decisions */
    int ucap[POLAR_CODE_STAGE + 1][POLAR_CODE_LENGTH];

    /* Node state vector */
    int ns[2 * POLAR_CODE_LENGTH - 1] = {0};

    /* Propagation parameters */
    unsigned node = 0;
    unsigned depth = 0;
    bool is_all_bits_decode = false;
    unsigned temp = 0;

    /* Traverse until all bits are decoded */
    while (!is_all_bits_decode) {
        /* Position of node in node state vector */
        const unsigned npos = POW2(depth) - 1U + node;

        /* Length of current sub-vector */
        temp = POW2(POLAR_CODE_STAGE - depth);

        /* Index of current node in node_type vector */
        const unsigned node_type_ind = node * temp;

        /* Check for leaf node */
        if (depth == POLAR_CODE_STAGE) {
            /* Check for frozen node and take decision */
            ucap[POLAR_CODE_STAGE][node] = 0;

            if (info_nodes[node] != 0 && L[POLAR_CODE_STAGE][0] < 0) {
                ucap[POLAR_CODE_STAGE][node] = 1;
            }

            node == POLAR_CODE_LENGTH - 1 ? (is_all_bits_decode = true)
                                          : (node /= 2, depth -= 1);
            continue;
        }

        /* Non-leaf nodes */
        switch (ns[npos]) {
        case 0: { /* Propagate to left child */
            /* f_minsum and storage */
            for (unsigned i_L = 0; i_L < temp / 2; i_L++) {
                L[depth + 1][i_L]
                    = f_macro(L[depth][i_L], L[depth][i_L + temp / 2]);
            }

            /* Next node: Left child */
            node *= 2;
            depth += 1;

            /* Incoming belief length for left child */
            temp /= 2;

            ns[npos] = 1;
            break;
        }
        case 1: { /* Propagate to right child */
            /* g_minsum and storage */
            for (unsigned i_L = 0; i_L < temp / 2; i_L++) {
                L[depth + 1][i_L]
                    = g_macro(ucap[depth + 1][i_L + node_type_ind],
                              L[depth][i_L],
                              L[depth][i_L + temp / 2]);
            }

            /* Next node: right child */
            node = node * 2 + 1;
            depth += 1;

            /* Incoming belief length for right child */
            temp /= 2;

            ns[npos] = 2;
            break;
        }
        case 2: { /* Propagate to parent node */
            /* Combine */
            for (unsigned i_L = 0, count = 0; i_L < temp; i_L++, count++) {
                ucap[depth][i_L + node_type_ind]
                    = count < temp / 2
                        ? ucap[depth + 1][i_L + node_type_ind]
                              ^ ucap[depth + 1][i_L + node_type_ind + temp / 2]
                        : ucap[depth + 1][i_L + node_type_ind];
            }

            node /= 2;
            depth -= 1;
            break;
        }
        default: {
            break;
        }
        }
    }  // end of while loop

    for (unsigned i_m = 0; i_m < K; i_m++) {
        msg_cap[i_m] = ucap[POLAR_CODE_STAGE][data_pos[i_m]];
    }
}