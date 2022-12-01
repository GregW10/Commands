#include <stdio.h>

unsigned long long factorial(unsigned long long num) {
	if (num <= 1)
		return 1;
	return num*factorial(num - 1);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Two arguments must be provided (including exec. name).\n");
		return 1;
	}
	char *ptr = *(argv + 1);
	unsigned long long number = 0;
	while (*ptr) {
		if (*ptr < 48 && *ptr > 57) {
			fprintf(stderr, "Non-numeric character '%c' found in number string provided.\n", *ptr);
			if (*ptr == '.')
				fprintf(stderr, "No dot can be included within the number, as only integer "
						"numbers must be provided.\n");
			return 1;
		}
		number *= 10;
		number += *ptr++ - 48;
	}
	printf("%i\n", factorial(number));
	return 0;
}

