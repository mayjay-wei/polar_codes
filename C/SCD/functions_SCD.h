#ifndef FUNCTIONS_SCD_
#define FUNCTIONS_SCD_


#define MAXQR           (31)
#define POLAR_CODE_LENGTH (1024)
#define POW2(X) (1 << (X))


int uni();

float randn(float mu, float sigma);

void swap_int(int *a, int *b);

int partition_int(int arr[], int low, int high);

void quickSort_int(int arr[], int low, int high);

void encode(int *u);

void decode(int *msg_cap, unsigned n, unsigned K, const int *LLR_Q,
            const int *info_nodes, const int *data_pos);

#endif