#include "stdbool.h"
// extern void svf_assert(bool);

int rand(int a)
{
    return a;
}

int main() {
//   int x, y;
//   x = 1;
//   y = 0;
//   switch (rand(x))
//   {
//     case 0:
//         x += 1;
//         break;
//     case 1:
//         x += y;
//         break;
//     case 2:
//         x -= y;
//         break;
//     default:
//         x++;
//         y++;
//         break;
//     }
//     svf_assert(x >= y);
//     return 0;
    int x = rand(1);
    if (x > 1) {
        return 1;
    } else {
        return 0;
    }
}