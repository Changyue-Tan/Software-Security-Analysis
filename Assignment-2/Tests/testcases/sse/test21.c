#include <stdbool.h>
extern void svf_assert(bool);

// Simple recursive function: returns sum from 1 to n
int sum_to_n(int n) {
    if (n == 0) {
        return 0;
    } else {
        return n + sum_to_n(n - 1); // recursive call
    }
}

int main() {
    int result = sum_to_n(5); // 5 + 4 + 3 + 2 + 1 + 0 = 15
    svf_assert(result == 15);
    return 0;
}
