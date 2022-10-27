#include "arithmetic.h"

int main(int argc, char **argv) {
	if (argc == 1) {
		fprintf(stderr, "Insufficient arguments.\n");
		return 1;
	}
	char **ptr = argv + 1;
	long double total = 0;
	bool is_floating = false;
	unsigned int dp = 10;
	for (int i = 1; i < argc; ++i, ++ptr) {
		if (!is_numeric(*ptr)) {
			if ((dp = get_dp(*ptr)) != (unsigned int) -1) {
				if (argc == 2) {
					fprintf(stderr, "Specifying decimal places cannot be the only argument.\n");
					return 1;
				}
				continue;
			}
			fprintf(stderr, "Argument \"%s\" is not numeric:\n", *ptr);
			print_non_numeric(*ptr);
			printf("\n... nor specifies decimal places (with \"dp=...\").\n");
			return 1;
		}
		total += get_fp(*ptr);
		is_floating = is_floating || is_fp(*ptr);
	}
	if (is_floating) printf(get_dp_str(&dp), total);
	else printf("%.0Lf\n", total);
	return 0;
}
