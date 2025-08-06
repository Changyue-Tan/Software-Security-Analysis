#include "stdbool.h"
extern void svf_assert(bool);
#define LEN 2

int main() {
    int a[LEN] = {0, 1};
    int *p  = a + LEN - 1;
    *p = 6131;
    svf_assert(a[LEN - 1] == 6131);
    return 0;
}
