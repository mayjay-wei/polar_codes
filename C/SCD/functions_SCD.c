#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "functions_SCD.h"

#include <string.h>

#define f_macro(L1, L2) (SIGN(L1) * SIGN(L2) * MIN(ABSL(L1), ABSL(L2)))
#define g_macro(u, L1, L2) \
    MIN(MAX((((1 - 2 * u) * L1) + L2), -(MAXQR + 1)), MAXQR)
// #define g_macro(u, L1, L2) (((1 - 2*u) * L1) + L2)
#define SIGN(x)   ((x > 0) - (x < 0))
#define ABSL(x)   (((x > 0) - (x < 0)) * x)
#define MIN(x, y) ((x < y) ? x : y)
#define MAX(x, y) ((x < y) ? y : x)

/*Uniform distribution number generator*/
int uni() { return rand() % 2; }

/*Gaussian random number generator*/
float randn(const float mu, const float sigma) {
    float U1, U2, W;
    static float X1, X2;
    static int call = 0;

    if (call == 1) {
        call = !call;
        return mu + sigma * X2;
    }

    do {
        U1 = -1 + (float)rand() / RAND_MAX * 2;
        U2 = -1 + (float)rand() / RAND_MAX * 2;
        W = U1 * U1 + U2 * U2;
    } while (W >= 1 || W <= 0);

    const float mult = sqrtf(-2.0f * logf(W) / W);
    X1 = U1 * mult;
    X2 = U2 * mult;

    call = !call;

    return mu + sigma * X1;
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

/*Encoder*/
void encode(int *u) {
    /* No of stages */
    /* log function */
    const int n = __builtin_ctz(POLAR_CODE_LENGTH);

    /* Stage after stage */
    for (int i_s = 0; i_s < n; i_s++) {
        /* Group after group in each stage */
        for (int i_g = 0; i_g < POW2(n - i_s - 1); i_g++) {
            const int del = POW2(i_s);
            const int base = POW2(i_s + 1) * i_g;

            /* Subgroup after subgroup */
            for (int i_sg = 0; i_sg < del; i_sg++) {
                u[base + i_sg] = u[base + i_sg] ^ u[base + i_sg + del];
            }
        }
    }
}

/* SC Decoder */
void decode(int *msg_cap, const unsigned n, const unsigned K, const int *LLR_Q,
            const int *info_nodes, const int *data_pos) {
    /* Beliefs */
    int L[n + 1][POLAR_CODE_LENGTH];

    /* Decisions */
    int ucap[n + 1][POLAR_CODE_LENGTH];

    /* Node state vector */
    int ns[2 * POLAR_CODE_LENGTH - 1] = {0};

    /* Belief initialisation */
    memcpy(L[0], LLR_Q, sizeof(int) * POLAR_CODE_LENGTH);

    /* Propogation parameters */
    unsigned node = 0;
    unsigned depth = 0;
    unsigned done = 0;
    unsigned npos = 0;
    unsigned temp = 0;

    /* Traverse till all bits are decoded */
    while (!done) {
        /* Position of node in node state vector */
        npos = POW2(depth) - 1U + node;

        /* Length of current sub-vector */
        temp = POW2(n - depth);

        /* Index of current node in node_type vector */
        const unsigned node_type_ind = node * temp;

        /* Check for leaf node */
        if (depth == n) {
            /* Check for frozen node and take decision */
            ucap[n][node] = 0;

            if (info_nodes[node] != 0 && L[n][0] < 0) {
                ucap[n][node] = 1;
            }

            node == POLAR_CODE_LENGTH - 1 ? (done = 1)
                                           : (node /= 2, depth -= 1);

        } else { /* Non-leaf nodes */
            /* Propogate to left child */
            if (ns[npos] == 0) {
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
            } else { /* Propagate to right child */
                if (ns[npos] == 1) {
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
                } else { /* Propagate to parent node */
                    /* Combine */
                    unsigned count = 0;
                    for (unsigned i_L = 0; i_L < temp; i_L++) {
                        if (count < temp / 2) {
                            ucap[depth][i_L + node_type_ind]
                                = ucap[depth + 1][i_L + node_type_ind]
                                ^ ucap[depth + 1]
                                      [i_L + node_type_ind + temp / 2];
                        } else {
                            ucap[depth][i_L + node_type_ind]
                                = ucap[depth + 1][i_L + node_type_ind];
                        }
                        count++;
                    }

                    node /= 2;
                    depth -= 1;
                }
            }
        }  // end of non-leaf node else
    }  // end of while loop

    for (unsigned i_m = 0; i_m < K; i_m++) {
        msg_cap[i_m] = ucap[n][data_pos[i_m]];
    }
}