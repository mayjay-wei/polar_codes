#ifndef FUNCTIONS_SCD_
#define FUNCTIONS_SCD_

#include <stdbool.h>

#define MAXQR             (31)
#define POW2(X)           (1 << (X))
#define POLAR_CODE_STAGE  (2)
#define POLAR_CODE_LENGTH (POW2(POLAR_CODE_STAGE))

bool uniformBinaryRandomNumber();

float randn(float mean, float std);

void swap_int(int *a, int *b);

int partition_int(int arr[], int low, int high);

void quickSort_int(int arr[], int low, int high);

void Encode(int *codeword);

void Decode(int *msg_cap, unsigned K, const int *LLR_Q, const bool *info_nodes,
            const int *data_pos);

#endif