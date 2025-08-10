#include "stdbool.h"
extern void svf_assert(bool);

#define SIZE 5

int main() {
    int arr[SIZE] = {1, 2, 3, 4, 5};
    int *p = arr;
    int sum = 0;
    for (int i = 0; i < SIZE; i++) {
        sum += *(p + i);
    }
    svf_assert(sum == 15);  // 1+2+3+4+5=15
    return 0;
}
