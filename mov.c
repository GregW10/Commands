//
// Created by mario on 21/07/2022.
//

#include <stdio.h>
#include <sys/stat.h>

typedef enum {
    false, true
} bool;

int main(int argc, char **argv) {
    char *src = *(argv + 1);
    char *dst = *(argv + 2);

    struct stat buff1 = {};
    struct stat buff2 = {};

    if (stat(src, &buff1) == -1) {
        fprintf(stderr, "Error: \"%s\" is not a valid file.", src);
        return -1;
    }
    stat(dst, &buff2);

    if (S_ISREG(buff1.st_mode) && S_ISDIR(buff2.st_mode)) {

    }

    return 0;
}