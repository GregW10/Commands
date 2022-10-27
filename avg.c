#include "arithmetic.h"

int main(int argc, char **argv) {
	if (argc == 1) {
		fprintf(stderr, "Insufficient arguments.\n");
		return 1;
	}
	char **ptr = argv + 1;
	long double total = 0;
	long double squared_total = 0;
	long double val;
	bool is_floating = false;
	bool sd_floating = false;
	bool has_dp = false;
	bool has_sd = false;
	unsigned int dp = 10;
	for (int i = 1; i < argc; ++i, ++ptr) {
		if (!is_numeric(*ptr)) {
			if (strcmp_c(*ptr, "sd") == 0) {
				if (argc == 2) {
					fprintf(stderr, "Standard deviation option cannot be the only argument.\n");
					return 1;
				}
				if (argc == 3 && has_dp) {
					fprintf(stderr, "Standard deviation option and specifying decimal places cannot be the only arguments.\n");
					return 1;
				}
				has_sd = true;
				continue;
			}
			else if ((dp = get_dp(*ptr)) != (unsigned int) -1) {
				if (argc == 2) {
					fprintf(stderr, "Specifying decimal places cannot be the only argument.\n");
					return 1;
				}
				if (argc == 3 && has_sd) {
					fprintf(stderr, "Standard deviation option and specifying decimal places cannot be the only arguments.\n");
					return 1;
				}
				has_dp = true;
				continue;
			}
			fprintf(stderr, "Argument \"%s\" is not numeric:\n", *ptr);
			print_non_numeric(*ptr);
			printf("\n... nor specifies decimal places (with \"dp=...\"), or specifies standard deviation option with \"sd\".\n");
			return 1;
		}
		total += (val = get_fp(*ptr));
		squared_total += val*val;
		is_floating = is_floating || is_fp(*ptr);
	}
	int divisor = argc - 1 - has_dp - has_sd;
	long double avg = total/divisor;
	long double sd = sqrtl(squared_total/divisor - avg*avg);
	is_floating = !(avg == ceill(avg));
	sd_floating = !(sd == ceill(sd));
	unsigned int zero = 0;
	if (is_floating) {
		if (has_sd) {
			printf(get_dp_sd_str(&dp, sd_floating ? &dp : &zero), avg, sd);
		}
		else {
			printf(get_dp_str(&dp), avg);
		}
	}
	else {
		if (has_sd) {
			printf(get_dp_sd_str(&zero, sd_floating ? &dp : &zero), avg, sd);
		}
		else {
			printf("%.0Lf\n", avg);
		}
	}
	return 0;
}
