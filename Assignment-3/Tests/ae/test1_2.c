#include "stdbool.h"
extern void svf_assert(bool);

int main() {
    int a = 6771;
    svf_assert(a == 6771);
    int *p  = &a;
    *p = 6131;
    svf_assert(a == 6131);
    return 0;
}
