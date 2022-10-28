#include "arithmetic.h"

int main(int argc, char **argv) {
	if (argc == 1) {
		fprintf(stderr, "Insufficient arguments.\n");
		return 1;
	}
	atexit(clean);
	char *path = get_path();
	long double prev;
	long double tot_prev = 1;
	int prev_count = 0;
	if (argc == 2) {
		if (!is_numeric(*(argv + 1)) && strcmp_c(*(argv + 1), "prev") != 0) {
			fprintf(stderr, "Argument \"%s\" is not numeric:\n", *(argv + 1));
			print_non_numeric(*(argv + 1));
			fputs("\n... nor specifies previous result using \"prev\".\n", stderr);
			return 1;
		}
		get_prev_num(path, &prev, NULL);
		tot_prev = prev;
		++prev_count;
	}
	char **ptr = argv + 1;
	long double product = 1;
	unsigned int dp = 10;
	for (int i = 1; i < argc; ++i, ++ptr) {
		if (!is_numeric(*ptr)) {
			if (strcmp_c(*ptr, "prev") == 0) {
				if (prev_count++ > 0) {
					tot_prev *= prev;
					continue;
				}
				get_prev_num(path, &prev, NULL);
				tot_prev = prev;
				continue;
			}
			else if ((dp = get_dp(*ptr)) != (unsigned int) -1) {
				if (argc == 2) {
					fprintf(stderr, "Specifying decimal places cannot be the only argument.\n");
					return 1;
				}
				continue;
			}
			fprintf(stderr, "Argument \"%s\" is not numeric:\n", *ptr);
			print_non_numeric(*ptr);
			fputs("\n... nor specifies decimal places (with \"dp=...\").\n", stderr);
			return 1;
		}
		product *= get_fp(*ptr);
	}
	product *= tot_prev;
	if (!(ceill(product) == product)) printf(get_dp_str(&dp), product);
	else printf("%.0Lf\n", product);
	write_num(path, product, dp);
}
