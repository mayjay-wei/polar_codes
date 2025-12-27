int uni ();

float randn (float mu, float sigma);

void swap_int(int* a, int* b);

unsigned int partition_int (int arr[], unsigned int low, unsigned int high);

void quickSort_int(int arr[], unsigned int low, unsigned int high);

int is_vec_mem(int in, int Nt, int N, int *data_pos_sorted);

void find_node_type(int *node_type, int N, int Nt, int in, int *data_pos_sorted, int *frozen_pos_sorted);

void encode(int *u, int N);

void decode(int *msg_cap, unsigned int n, unsigned int N, unsigned int K, int *LLR_Q, int *info_nodes, int *data_pos, int *node_type);
