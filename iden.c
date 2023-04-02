#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef enum boolean {
	false, true
} bool;

char *buffer = NULL;
char *buffer2 = NULL;

void clean(void) {
	if (buffer)
		free(buffer);
	if (buffer2)
		free(buffer2);
}

bool is_numeric_c(char c) {
	return c <= 57 && c >= 48;
}

size_t to_size_t(const char *str) {
	if (!str || !*str)
		return -1;
	size_t retval = 0;
	while (*str) {
		if (!is_numeric_c(*str))
			return -1;
		retval *= 10;
		retval += *str++;
	}
	return retval;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Error: invalid number of command-line arguments.\n"
				"Usage: ./iden [opt:RAM_limit] <file1> <file2> [opt:further_files...]");
		return 1;
	}
	size_t lim;
	char **ptr;
	size_t counter;
	if ((lim = to_size_t(*(argv + 1))) == (size_t) -1) {
		lim = 1000000000;
		ptr = argv + 1;
		counter = argc - 2;
	} else {
		if (argc < 4) {
			fprintf(stderr, "Error: invalid number of command-line arguments.\n"
					"Usage: ./iden [opt:RAM_limit] <file1> <file2> [opt:further_files...]");
			return 1;
		}
		ptr = argv + 2;
		counter = argc - 3;
	}
	struct stat buff = {0};
	atexit(clean);
	if (stat(*ptr, &buff) == -1) {
		fprintf(stderr, "Error: first file \"%s\" does not exist.\n", *ptr);
		return 1;
	}
	const size_t f_size = buff.st_size;
	size_t fcounter;
	printf("File identical to \"%s\"?\n", *ptr);
	const char *b1;
	const char *b2;
	if (buff.st_size < lim) {
		FILE *fp = fopen(*ptr++, "rb");
		if (!fp) {
			fprintf(stderr, "Error opening file: \"%s\"\n", *--ptr);
			return 1;
		}
		buffer = malloc(buff.st_size);
		buffer2 = malloc(buff.st_size);
		if (!buffer || !buffer2) {
			fprintf(stderr, "Memory allocation error.\n");
			return 1;
		}
		fread(buffer, buff.st_size, 1, fp);
		fclose(fp);
        	loop_start:
		for (; counter --> 0; ++ptr) {
			fp = fopen(*ptr, "rb");
			if (!fp) {
				fprintf(stderr, "Error opening file: \"%s\"\n", *ptr);
				return 1;
			}
			if (stat(*ptr, &buff) == -1) {
				fprintf(stderr, "Error: file \"%s\" does not exist.\n", *ptr);
				return 1;
			}
            		if (buff.st_size != f_size) {
                		printf("\"%s\": NO\n", *ptr);
                		continue;
            		}
			fread(buffer2, buff.st_size, 1, fp);
			fclose(fp);
            		fcounter = f_size;
            		b1 = buffer;
            		b2 = buffer2;
            		while (fcounter --> 0) {
                		if (*b1++ != *b2++) {
                    		printf("\"%s\": NO\n", *ptr++);
                    		goto loop_start;
                		}
			}
			printf("\"%s\": YES\n", *ptr);
		}
	} else {
		FILE *fp1 = fopen(*ptr++, "rb");
        	if (!fp1) {
            	fprintf(stderr, "Error opening file: \"%s\"\n", *--ptr);
            	return 1;
        	}
        	FILE *fp2;
        	loop2_start:
        	for (; counter --> 0; ++ptr) {
            		if (stat(*ptr, &buff) == -1) {
                		fprintf(stderr, "Error: file \"%s\" does not exist.\n", *ptr);
                		return 1;
            		}
            		if (buff.st_size != f_size) {
                		printf("\"%s\": NO\n", *ptr);
                		continue;
            		}
            		fp2 = fopen(*ptr, "rb");
            		if (!fp2) {
                		fprintf(stderr, "Error opening file: \"%s\"\n", *ptr);
                		return 1;
            		}
            		fcounter = f_size;
            		while (fcounter --> 0) {
                		if (fgetc(fp1) != fgetc(fp2)) {
                    		printf("\"%s\": NO\n", *ptr++);
                    		goto loop2_start;
                		}
            		}
            		fclose(fp2);
            		fseek(fp1, 0, SEEK_SET);
            		printf("\"%s\": YES\n", *ptr);
        	}
        	fclose(fp1);
	}
	return 0;
}
