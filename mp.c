//
// Created by mario on 21/07/2022.
//

#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <wchar.h>

#ifdef _WIN32
#include <shlobj.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif

#define MAX_INT -2147483648

typedef enum {
    false, true
} bool;

size_t strlen_c(const char *restrict str) {
    if (str == NULL || *str == 0) {
        return 0;
    }
    size_t count = 0;
    while (*str++) ++count;
    return count;
}

size_t strcpy_c(char *restrict destStr, const char *restrict srcStr) {
    if (destStr == NULL || srcStr == NULL || *srcStr == 0) {
        return 0;
    }
    size_t count = 0;
    while ((*destStr++ = *srcStr++)) ++count;
    return count;
}

size_t strcat_c(char *restrict destStr, const char *restrict srcStr) {
    if (destStr == NULL || srcStr == NULL || *srcStr == 0) {
        return 0;
    }
    if (!*destStr) goto start;
    size_t count = 0;
    while (*++destStr);
    start:
    while ((*destStr++ = *srcStr++)) ++count;
    return count;
}

int strcmp_c(const char *str1, const char *str2) {
    if (str1 == NULL || str2 == NULL) {
        return MAX_INT;
    }
    if (*str1 == 0 || *str2 == 0) {
        return *str1 - *str2;
    }
    while (*str1 && *str2) {
        if (*str1++ != *str2++) {
            return *str1 - *str2;
        }
    }
    return *str1 - *str2;
}

bool endswith(const char *str, const char *end) {
    if (str == NULL || *str == 0 || end == NULL || *end == 0) {
        return false;
    }
    size_t len = strlen_c(str);
    size_t end_len = strlen_c(end);
    if (end_len > len) {
        return false;
    }
    const char *ptr = str + len - end_len;
    while (*ptr) {
        if (*ptr++ != *end++) {
            return false;
        }
    }
    return true;
}

void *memset_c(void *str, char c, size_t n) {
    char *ptr = str;
    while (n-- > 0) *ptr++ = c;
    return str;
}

void free_ptrs(size_t numPointers, ...) {
    if (numPointers == 0) {
        return;
    }
    va_list ptr;
    va_start(ptr, numPointers);
    while (numPointers-- > 0) {
        free(va_arg(ptr, void *));
    }
    va_end(ptr);
}

char *getHomePath() {
    char *retptr;
#ifdef _WIN32
    retptr = malloc(PATH_MAX);
    if (SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, retptr) != S_OK) {
        return NULL;
    }
    retptr = realloc(retptr, strlen_c(retptr) + 1);
#else
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    retptr = pwd->pw_dir;
#endif
    return retptr;
}

int main() {
    char *homePath = getHomePath();
    size_t HPlen = strlen_c(homePath);
    char *srcDir = malloc(HPlen + 12);
    char *destDir = malloc(HPlen + 17);
    strcpy_c(srcDir, homePath);
#ifdef _WIN32
    strcat_c(srcDir, "\\Downloads");
#else
    strcat_c(srcDir, "/Downloads");
#endif
    strcpy_c(destDir, homePath);
    free(homePath);
#ifdef _WIN32
    strcat_c(destDir, "\\ProjectReading");
#else
    strcat_c(destDir, "/ProjectReading");
#endif
    struct stat64 buffer = {};
    if (stat64(srcDir, &buffer) == -1) {
        free_ptrs(2, srcDir, destDir);
        fprintf(stderr, "Downloads folder does not exist.\n");
        return -1;
    }
    if (!S_ISDIR(buffer.st_mode)) {
        free_ptrs(2, srcDir, destDir);
        fprintf(stderr, "Downloads folder is actually a file.\n");
        return -1;
    }
    if (stat64(destDir, &buffer) == -1) {
#ifdef _WIN32
        if (mkdir(destDir) == -1) {
#else
        if (mkdir(destDir, S_IRWXG | S_IRWXU | S_IRWXO) == -1) {
#endif
            free_ptrs(2, srcDir, destDir);
            fprintf(stderr, "Destination directory \"%s\" does not exist nor could be created.\n", destDir);
            return -1;
        }
    }
    else if (!S_ISDIR(buffer.st_mode)) { // in case the dest. dir. is a file
        srand((int) time(NULL));
        char *tempDir = malloc(HPlen + 27);
        memset_c(tempDir, 0, HPlen + 27);
        strcpy_c(tempDir, destDir);
        char *endptr = tempDir + HPlen + 15;
        unsigned count = 0;
        do {
            for (char i = 0; i < 10; ++i, ++endptr) {
                *endptr = (rand() % 10) + 48;
            }
            endptr -= 10;
        } while(rename(destDir, tempDir) == -1 && count++ < 1000000); // attempt to rename the file
        struct stat64 tempBuff = {};
        if (stat64(tempDir, &tempBuff) == -1) {
            free_ptrs(3, srcDir, destDir, tempDir);
            fprintf(stderr, "Destination file is not a directory, nor could it be renamed.\n");
            return -1;
        }
#ifdef _WIN32
        if (mkdir(destDir) == -1) {
#else
        if (mkdir(destDir, S_IRWXG | S_IRWXU | S_IRWXO) == -1) {
#endif
            free_ptrs(3, srcDir, destDir, tempDir);
            fprintf(stderr, "Destination file was not a directory, so was renamed to \"%s\", but destination\n"
                            "directory could still not be created.\n", tempDir);
            return -1;
        }
        free(tempDir);
    }
    struct dirent *entry;
    DIR *dir = opendir(srcDir);
    time_t mtime = 0;
    size_t PDFlen = 260 + HPlen + 12 + 1;
    char *PDF = malloc(PDFlen);
    char finalPDF[264] = {0};
    memset_c(PDF, 0, PDFlen);
    while ((entry = readdir(dir)) != NULL) {
        if (endswith(entry->d_name, ".pdf")) {
            strcpy_c(PDF, srcDir); strcat_c(PDF, "\\"); strcat_c(PDF, entry->d_name);
            stat64(PDF, &buffer);
            FILE *fp = fopen(entry->d_name, "rb");
            if (fp == NULL) {
                fprintf(stderr, "NULL again.\n");
                return -1;
            }
            fclose(fp);
            printf("what: %d\n", entry->d_name[2]);
            if (buffer.st_mtime > mtime) {
                mtime = buffer.st_mtime;
#ifdef _WIN32
                strcpy_c(finalPDF, "\\");
#else
                strcpy_c(finalPDF, "/");
#endif
                strcat_c(finalPDF, entry->d_name);
            }
        }
    }
    closedir(dir);
    if (*PDF == 0) {
        free_ptrs(3, srcDir, destDir, PDF);
        fprintf(stderr, "No PDF file found in Downloads folder.\n");
        return -1;
    }
    printf("PDF: %s\n", PDF);
    free(PDF);
    size_t flen = strlen_c(finalPDF);
    char *srcFile = malloc(HPlen + 12 + flen + 1);
    char *destFile = malloc(HPlen + 17 + flen + 1);
    strcpy_c(srcFile, srcDir); strcat_c(srcFile, finalPDF);
    strcpy_c(destFile, destDir); strcat_c(destFile, finalPDF);
    FILE *src = fopen(srcFile, "rb");
    FILE *dest = fopen(destFile, "wb");
    if (src == NULL) {
        free_ptrs(4, srcDir, destDir, srcFile, destFile);
        fprintf(stderr, "Error opening source file: %s\n", srcFile);
        return -1;
    }
    if (dest == NULL) {
        free_ptrs(4, srcDir, destDir, srcFile, destFile);
        fprintf(stderr, "Error opening destination file: %s\n", destFile);
        return -1;
    }
    int c;
    while ((c = fgetc(src)) != EOF) {
        fputc(c, dest);
    }
    fclose(src); fclose(dest);
    free(destFile);
    if (remove(srcFile)) {
        free_ptrs(3, srcDir, destDir, srcFile);
        fprintf(stderr, "Error deleting source file: %s\n", srcFile);
        return -1;
    }
    free_ptrs(3, srcFile, srcDir, destDir);
    printf("Successfully moved: %s\n", &finalPDF[1]);
    return 0;
}