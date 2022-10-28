#include "arithmetic.h"

int main(int argc, char **argv) {
	if (argc == 1) {
		fprintf(stderr, "Insufficient arguments.\n");
		return 1;
	}
	atexit(clean);
	char *path = get_path();
	long double prev;
	long double tot_prev = 0;
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
	if (argc == 3) {
		bool prev1 = strcmp_c(*(argv + 1), "prev") == 0;
		bool prev2 = strcmp_c(*(argv + 2), "prev") == 0;
		bool num1 = is_numeric(*(argv + 1));
		bool num2 = is_numeric(*(argv + 2));
		if (!prev1 && !prev2 && !num1 && !num2) {
			fprintf(stderr, "Arguments \"%s\" and \"%s\" are not numeric:\n", *(argv + 1), *(argv + 2));
			print_non_numeric(*(argv + 1));
			print_non_numeric(*(argv + 2));
			fputs("\n... nor specify previous result using \"prev\".\n", stderr);
			return 1;
		}
		prev_count = prev1 + prev2;
		if (prev_count + num1 + num2 == 1) {
			get_prev_num(path, &prev, NULL);
			tot_prev = prev;
		}
		else
			prev_count = 0;
	}
	char **ptr = argv + 1;
	long double sum = 0;
	unsigned int dp = 10;
	for (int i = 1; i < argc; ++i, ++ptr) {
		if (!is_numeric(*ptr)) {
			if (strcmp_c(*ptr, "prev") == 0) {
				if (prev_count++ > 0) {
					tot_prev += prev;
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
			fputs("\n... nor specifies decimal places (with \"dp=...\"), or previous result with \"prev\".\n", stderr);
			return 1;
		}
		sum += get_fp(*ptr);
	}
	sum += tot_prev;
	if (!(ceill(sum) == sum)) printf(get_dp_str(&dp), sum);
	else printf("%.0Lf\n", sum);
	write_num(path, sum, dp);
}
