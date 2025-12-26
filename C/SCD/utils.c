#include <stddef.h>

size_t bitReverseIndex(const size_t index, const size_t stage) {
    size_t reversed_index = 0;
    for (size_t i = 0; i < stage; i++){
        reversed_index <<= 1;
        reversed_index |= (index >> i) & 1;
    }
    return reversed_index;
}

void bitReverseArray(int *arr, const size_t length, const size_t stage) {
    for (size_t i = 0; i < length; i++) {
        const size_t rev_i = bitReverseIndex(i, stage);
        if (i < rev_i) {
            const int temp = arr[i];
            arr[i] = arr[rev_i];
            arr[rev_i] = temp;
        }
    }
}

