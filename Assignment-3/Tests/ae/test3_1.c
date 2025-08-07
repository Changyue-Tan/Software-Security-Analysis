#include "stdbool.h"
extern void svf_assert(bool);

int fx(void) {
	int x = 1;
	return x;
}

int main() {
	int x, y;
	x = 1;
	y = 0;
	switch (fx()) {
	case 0: x += 1; break;
	case 1: x += y; break;
	case 2: x -= y; break;
	default:
		x++;
		y++;
		break;
	}
	svf_assert(x >= y);
	return 0;
}