//
// Created by mario on 22/07/2022.
//

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#endif

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Invalid number of arguments provided.\n");
        return -1;
    }

#ifdef _WIN32
    int Argc;
    LPWSTR *dir = CommandLineToArgvW(GetCommandLineW(), &Argc);

    if (dir == NULL) {
        fprintf(stderr, "Error getting command line arguments.\n");
        return -1;
    }

    LPWSTR path = malloc(wcslen(*(dir + 1))*sizeof(wchar_t) + 6);
    wcscpy(path, *(dir + 1));
    DWORD fileAttribute = GetFileAttributesW(path);
    wprintf(L"fileAttribute: %d, invalid: %d, DIR: %d\n", fileAttribute, INVALID_FILE_ATTRIBUTES, FILE_ATTRIBUTE_DIRECTORY);
    wprintf(L"path: %ls\n", path);
    wprintf(L"GetLastError(): %d\n", GetLastError());
    wcscat(path, L"\\*");
#else
    struct stat64 buffer = {};
#endif

#ifdef _WIN32
    if (fileAttribute == INVALID_FILE_ATTRIBUTES) {
#else
    if (stat64(*(argv + 1), &buffer) == -1) {
#endif
        fprintf(stderr, "The given file does not exist.\n");
        return -1;
    }

#ifdef _WIN32
    if ((fileAttribute & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
#else
    if (!S_ISDIR(buffer.st_mode)) {
#endif
        fprintf(stderr, "The given path does not represent a directory.\n");
        return -1;
    }

#ifdef _WIN32
    WIN32_FIND_DATAW entry = {};
    HANDLE hFound;
    if ((hFound = FindFirstFileW(path, &entry)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening directory.\n");
        return -1;
    }
    BOOL moreFiles = TRUE;
#else
    struct dirent *entry;
    DIR *dir = opendir(*(argv + 1));
    struct stat64 temp = {};
    char *ptr;
#endif
    printf("Last Modified Date & Time\tType\tSize (bytes)\tFilename\n\n");
#ifdef _WIN32
    while (moreFiles != 0) {
        wprintf(L"File: %ls\n", entry.cFileName);
        moreFiles = FindNextFileW(hFound, &entry);
    }
    FindClose(hFound);
#else
    while ((entry = readdir(dir)) != NULL) {
        stat64(entry->d_name, &temp);
        ptr = ctime(&temp.st_mtime);
        *(ptr + 24) = 0;
        printf("%s\t%s\t%zu\t\t%s\n\n", ptr, "TYPE", temp.st_size, entry->d_name);
    }
    closedir(dir);
#endif

    return 0;
}