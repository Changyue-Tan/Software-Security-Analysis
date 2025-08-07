#include "stdbool.h"
extern void svf_assert(bool);

int main() {
    int a[1] = {6771};
    int *p  = a;
    *p = 6131;
    svf_assert(*p == 6131);
    return 0;
}
