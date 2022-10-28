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
	long double tot_prev_squared = 0;
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
		tot_prev_squared = prev*prev;
		++prev_count;
	}
	char **ptr = argv + 1;
	long double total = 0;
	long double squared_total = 0;
	long double val;
	bool sd_floating = false;
	bool has_dp = false;
	bool has_sd = false;
	unsigned int dp = 10;
	for (int i = 1; i < argc; ++i, ++ptr) {
		if (!is_numeric(*ptr)) {
			if (strcmp_c(*ptr, "prev") == 0) {
				if (prev_count++ > 0) {
					tot_prev += prev;
					tot_prev_squared += prev*prev;
					continue;
				}
				get_prev_num(path, &prev, NULL);
				tot_prev = prev;
				tot_prev_squared = prev*prev;
				continue;
			}
			else if (strcmp_c(*ptr, "sd") == 0) {
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
			fputs("\n... nor specifies decimal places (with \"dp=...\"), or specifies standard deviation option with \"sd\", or previous result using \"prev\".\n", stderr);
			return 1;
		}
		total += (val = get_fp(*ptr));
		squared_total += val*val;
	}
	total += tot_prev;
	squared_total += tot_prev_squared;
	int divisor = argc - !(argc == 2 && prev_count == 1) - has_dp - has_sd;
	long double avg = total/divisor;
	long double sd = sqrtl(squared_total/divisor - avg*avg);
	sd_floating = !(sd == ceill(sd));
	unsigned int zero = 0;
	if (!(avg == ceill(avg))) {
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
	write_num(path, avg, dp);
}
