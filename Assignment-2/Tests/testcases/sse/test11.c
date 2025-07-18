#include <stdbool.h>
extern void svf_assert(bool);

int main() {
    int x = 5;
    int y;

    if (x > 3) {
        y = 10;
    } else {
        y = 20;
    }

    // The value of y should be 10 because x > 3 is true
    svf_assert(y == 10);
}
