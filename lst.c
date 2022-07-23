//
// Created by mario on 22/07/2022.
//

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#else
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#endif

#ifdef _WIN32

static inline int getFormattedTime(const SYSTEMTIME *sysTime, wchar_t *str) {
    if (sysTime == NULL || str == NULL) {
        return 1;
    }
    switch (sysTime->wDayOfWeek) {
        case 0:
            *str++ = L'S'; *str++ = L'u'; *str++ = L'n';
            break;
        case 1:
            *str++ = L'M'; *str++ = L'o'; *str++ = L'n';
            break;
        case 2:
            *str++ = L'T'; *str++ = L'u'; *str++ = L'e';
            break;
        case 3:
            *str++ = L'W'; *str++ = L'e'; *str++ = L'd';
            break;
        case 4:
            *str++ = L'T'; *str++ = L'h'; *str++ = L'u';
            break;
        case 5:
            *str++ = L'F'; *str++ = L'r'; *str++ = L'i';
            break;
        case 6:
            *str++ = L'S'; *str++ = L'a'; *str++ = L't';
            break;
    } *str++ = 32;
    //sysTime->wDay < 10 ? swprintf(str, 3, L"0%d", sysTime->wDay) : swprintf(str, 3, L"%d", sysTime->wDay); str += 3; *str++ = 32;
    swprintf(str, 3, L"%2d", sysTime->wDay); *(str += 2) = 32; ++str;
    switch (sysTime->wMonth) {
        case 1:
            swprintf(str, 4, L"Jan");
            break;
        case 2:
            swprintf(str, 4, L"Feb");
            break;
        case 3:
            swprintf(str, 4, L"Mar");
            break;
        case 4:
            swprintf(str, 4, L"Apr");
            break;
        case 5:
            swprintf(str, 4, L"May");
            break;
        case 6:
            swprintf(str, 4, L"Jun");
            break;
        case 7:
            swprintf(str, 4, L"Jul");
            break;
        case 8:
            swprintf(str, 4, L"Aug");
            break;
        case 9:
            swprintf(str, 4, L"Sep");
            break;
        case 10:
            swprintf(str, 4, L"Oct");
            break;
        case 11:
            swprintf(str, 4, L"Nov");
            break;
        case 12:
            swprintf(str, 4, L"Dec");
            break;
    } *(str += 3) = 32; str++;
    swprintf(str, 14, L"%d %2d:%2d:%2d", sysTime->wYear, sysTime->wHour, sysTime->wMinute, sysTime->wSecond);
    if (*(str += 5) == 32) {
        *str = 48;
    }
    if (*(str += 3) == 32) {
        *str = 48;
    }
    if (*(str += 3) == 32) {
        *str = 48;
    }
    return 0;
}

#endif

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Invalid number of arguments provided.\n");
        return -1;
    }

#ifdef _WIN32
    _setmode(_fileno(stdout), _O_U16TEXT);
    int Argc;
    LPWSTR *dir = CommandLineToArgvW(GetCommandLineW(), &Argc);

    if (dir == NULL) {
        fprintf(stderr, "Error getting command line arguments.\n");
        return -1;
    }

    LPWSTR path = malloc(wcslen(*(dir + 1))*sizeof(wchar_t) + 6);
    wcscpy(path, *(dir + 1));
    LocalFree(dir);
    DWORD fileAttribute = GetFileAttributesW(path);
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
    fwprintf(stdout, L"%24ls %13ls %7ls  %-ls", L"Last Modified",  L"Size (bytes)", L"Type", L"Filename\n");
#ifdef _WIN32
    SYSTEMTIME sysTime = {};
    wchar_t time[25] = {0};
    while (moreFiles != 0) {
        FileTimeToSystemTime(&entry.ftLastAccessTime, &sysTime);
        getFormattedTime(&sysTime, time);
        wprintf(L"%ls  %12zu", time, (((size_t) entry.nFileSizeHigh) << 32) + entry.nFileSizeLow);
        (entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY ? wprintf(L"\tFolder\t") : wprintf(L"\t  File\t");
        wprintf(L"%ls\n", entry.cFileName);
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