#include <stdio.h>
#include <math.h>

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
	fputc('\n', stderr);
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

char *get_dp_sd_str(unsigned int *dp, unsigned int *dp_sd) {
	static char ret[36]; // only need 35 chars, but best to allocate multiple of 4
	sprintf(ret, "%%.%iLf +/- %%.%iLf\n", *dp, *dp_sd);
	return ret;
}

long long strcmp_c(const char *restrict s1, const char *restrict s2) {
	if (s1 == NULL || s2 == NULL)
		return -128;
	while (*s1 && *s2) {
		if (*s1++ != *s2++)
			return *s1 - *s2;
	}
	return *s1 - *s2;
}
