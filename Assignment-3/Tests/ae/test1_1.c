#include "stdbool.h"
extern void svf_assert(bool);
#define LEN 2

int main() {
    int a[LEN] = {6771, 4128};
    int *p  = a + 1;
    *p = 6131;
    svf_assert(a[LEN - 1] == 6131);
    return 0;
}
