#include <stdio.h>
#include <math.h>

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
void *malloc(size_t size);
void free(void *);
#else
#include <windows.h>
#include <shlobj.h>
#endif

_Noreturn void exit(int);
int atexit(void (*)(void));

typedef enum {
	false, true
} bool;

void free_first(void *ptr, bool unregister) {
	static void *first = NULL;
	if (unregister)
		first = NULL;
	if (first)
		free(first);
	else
		first = ptr;
}

void free_second(void *ptr, bool unregister) {
	static void *second = NULL;
	if (unregister)
		second = NULL;
	if (second)
		free(second);
	else
		second = ptr;
}

void clean(void) {
	free_first(NULL, false);
	free_second(NULL, false);
}

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

size_t strlen_c(const char *str) {
	if (str == NULL)
		return 0;
	size_t count = 0;
	while (*str++) ++count;
	return count;
}

char *strcpy_c(char *dst, const char *restrict src) {
	if (dst == NULL || src == NULL)
		return NULL;
	char *cpy = dst;
	while (*src) *dst++ = *src++;
	*dst = 0;
	return cpy;
}

char *strcat_c(char *dst, const char *restrict src) {
	if (dst == NULL || src == NULL)
		return NULL;
	if (*src == 0)
		return dst;
	char *cpy = dst;
	if (*dst == 0)
		goto start;
	while (*++dst);
	start:
	while (*src) *dst++ = *src++;
	*dst = 0;
	return cpy;
}

char *chcat_c(char *dst, char ch) {
	if (dst == NULL)
		return NULL;
	char *cpy = dst;
	if (*dst == 0)
		goto start;
	while (*++dst);
	start:
	*dst++ = ch;
	*dst = 0;
	return cpy;
}

long long strcmp_c(const char *restrict s1, const char *restrict s2) {
	if (s1 == NULL || s2 == NULL)
		return -128;
	while (*s1 && *s2) {
		if (*s1 != *s2)
			return *s1 - *s2;
		++s1; ++s2;
	}
	return *s1 - *s2;
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
			if (point_count++ < 1) {
				fputc('~', stderr);
				continue;
			}
		}
		fputc('^', stderr);
	}
}

long double get_fp(const char *str) {
	if (!is_numeric(str))
		exit(1);
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

char *get_home_path(void) {
#ifndef _WIN32
	char *home_path;
	struct passwd *pwd = getpwuid(getuid());
	if (!pwd)
		return NULL;
	home_path = malloc(sizeof(char)*(strlen_c(pwd->pw_dir) + 1));
	if (!home_path)
		return NULL;
	free_first(home_path, false);
	strcpy_c(home_path, pwd->pw_dir);
	return home_path;
#else
	static char path[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, path) != S_OK)
		return NULL;
	return path;
#endif
}

char file_sep(void) {
#ifndef _WIN32
	return '/';
#else
	return '\\';
#endif
}

char *get_path(void) {
	char *home_path = get_home_path();
	if (home_path == NULL)
		return NULL;
	char *path = malloc(sizeof(char)*(strlen_c(home_path) + 13));
	if (path == NULL)
		return NULL;
	free_second(path, false);
	strcpy_c(path, home_path);
#ifndef _WIN32
	free(home_path);
	free_first(NULL, true);
#endif
	chcat_c(path, file_sep());
	strcat_c(path, "prevnum.dat");
	return path;
}

void get_prev_num(const char *path, long double *num, unsigned int *dp) {
	if (path == NULL || num == NULL)
		return;
#ifndef _WIN32
	struct stat buffer = {0};
	if (stat(path, &buffer) == -1) {
		fprintf(stderr, "Previous number non-existent. Use \"mul\", \"add\" or \"avg\" command first.\n");
		exit(1);
	}
	if (buffer.st_size != sizeof(long double) + sizeof(unsigned int) || !S_ISREG(buffer.st_mode)) {
		fprintf(stderr, "Previous number not in correct format. Use \"mul\", \"add\" or \"avg\" command first.\n");
		exit(1);
	}
#else
	WIN32_FIND_DATAA buffer = {0};
	HANDLE handle;
	if ((handle = FindFirstFileA(path, &buffer)) == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Previous number non-existent. Use \"mul\", \"add\" or \"avg\" command first.\n");
		exit(1);
	} // below, FILE_ATTRIBUTE_NORMAL is only valid when on its own, so no need to check for bitwise combinations
	if (buffer.dwFileAttributes != FILE_ATTRIBUTE_NORMAL || (buffer.nFileSizeHigh << (sizeof(DWORD)*8)) + buffer.nFileSizeLow != sizeof(long double) + sizeof(unsigned int)) {
		fprintf(stderr, "Previous number not in correct format. Use \"mul\", \"add\" or \"avg\" command first.\n");
		exit(1);
	}
	FindClose(handle);
#endif
	FILE *fp = fopen(path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error retrieving previous number.\n");
		exit(1);
	}
	fread(num, sizeof(long double), 1, fp);
	if (dp)
		fread(dp, sizeof(unsigned int), 1, fp);
	fclose(fp);
}

_Noreturn void write_num(char *path, long double num, unsigned int dp) {
	FILE *fp = fopen(path, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Current result could not be stored. Subsequent calls to \"add\", \"mul\" or \"avg\" will not be able to use the current result.\n");
		perror("Error type");
		exit(1);
	}
	fwrite(&num, sizeof(long double), 1, fp);
	fwrite(&dp, sizeof(unsigned int), 1, fp);
	free(path); // path is guaranted to have been dynamically allocated
	free_second(NULL, true);
	fclose(fp);
	exit(0);
}
