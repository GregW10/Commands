#include <stdio.h>

_Noreturn void abort(void);

typedef enum {
	false, true
} bool;

static inline bool isdigit_c(char ch) {
	return ch >= 48 && ch <= 57;
}

bool is_numeric(const char *str) {
	if (str == NULL || *str == 0)
		return false;
	if (*str == '-') {
		if (*++str == 0)
			return false;
	}
	size_t point_count = 0;
	for (; *str; ++str) {
		if (!isdigit_c(*str)) {
			if (*str == '.') {
				if (++point_count <= 1)
					continue;
			}
			return false;
		}
	}
	return true;
}

void print_non_numeric(const char *str) {
	if (str == NULL || is_numeric(str))
		return;
	fputs(str, stderr);
	if (*str == '-') {
		fputc('~', stderr);
		++str;
	}
	size_t point_count = 0;
	for (; *str; ++str) {
		if (isdigit_c(*str)) {
			fputc('~', stderr);
			continue;
		}
		else if (*str == '.') {
			if (point_count++ <= 1) {
				fputc('~', stderr);
				continue;
			}
		}
		fputc('^', stderr);
	}
}

long double get_fp(const char *str) {
	if (!is_numeric(str))
		abort();
	bool negative = false;
	if (*str == '-') {
		negative = true;
		++str;
	}
	long double ret = 0;
	size_t decimal_count = 0;
	bool after_point = false;
	for (; *str; ++str) {
		if (*str == '.') {
			after_point = true;
			continue;
		}
		if (after_point)
			++decimal_count;
		ret *= 10;
		ret += *str - 48;
	}
	long double divisor = 1;
	while (decimal_count --> 0)
		divisor *= 10;
	return negative ? -(ret/divisor) : ret/divisor;
}

bool is_fp(const char *str) {
	if (!is_numeric(str))
		return false;
	while (*str) {
		if (*str++ == '.') {
			if (!*str)
				return false;
			return true;
		}
	}
	return false;
}

unsigned int get_dp(const char *str) {
	if (str == NULL)
		return -1;
	if (*str++ != 'd')
		return -1;
	if (*str++ != 'p')
		return -1;
	if (*str++ != '=')
		return -1;
	if (!is_numeric(str))
		return -1;
	return (unsigned int) get_fp(str);
}

char *get_dp_str(unsigned int *dp) {
	static char ret[16];
	sprintf(ret, "%%.%iLf\n", *dp);
	return ret;
}

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
	printf("dp = %d\n", dp);
	return 0;
}