#include "arithmetic.h"

int main(void) {
	atexit(clean);
	char *path = get_path();
	long double num;
	unsigned int dp;
	get_prev_num(path, &num, &dp);
	if (!(ceill(num) == num)) printf(get_dp_str(&dp), num);
	else printf("%.0Lf\n", num);
	return 0;
}