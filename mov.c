//
// Created by mario on 21/07/2022.
//

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <shlobj.h>

#define ISDIR(attr) (attr & FILE_ATTRIBUTE_DIRECTORY)

#define MERGE_DWORDS(dwHigh, dwLow) ((((size_t) (dwHigh)) << (sizeof(DWORD)*8)) + (dwLow))

#define MIN_INT -2147483648

const wchar_t prePath[] = L"\\prePaths.dat";

typedef enum {
    false, true
} bool;

size_t wcslen_c(const wchar_t *str) {
    if (str == NULL || *str == 0) {
        return 0;
    }
    size_t count = 0;
    while (*str++) ++count;
    return count;
}

const wchar_t *wcscpy_c(wchar_t *destStr, const wchar_t *srcStr) {
    if (destStr == NULL || srcStr == NULL || *srcStr == 0) {
        return destStr;
    }
    while ((*destStr++ = *srcStr++));
    return destStr;
}

const wchar_t *wcscat_c(wchar_t *destStr, const wchar_t *srcStr) {
    if (destStr == NULL || srcStr == NULL || *srcStr == 0) {
        return destStr;
    }
    if (*destStr == 0) goto start;
    while (*++destStr);
    start:
    while ((*destStr++ = *srcStr++));
    return destStr;
}

int wcscmp_c(const wchar_t *str1, const wchar_t *str2) {
    if (str1 == NULL || str2 == NULL) {
        return MIN_INT;
    }
    while (*str1 && *str2) {
        if (*str1++ != *str2++) {
            return *str1 - *str2;
        }
    }
    return *str1 - *str2;
}

int wcsupr_c(wchar_t *str) {
    if (str == NULL || *str == 0) {
        return 1;
    }
    while (*str) {
        if (*str <= 122 && *str >= 97) {
            *str -= 32;
        }
        ++str;
    }
    return 0;
}

static inline int file_exists(const wchar_t *from, const wchar_t *to) {
    if (from == NULL || to == NULL || *from == 0 || *to == 0) {
        return 1;
    }
    DWORD fAttr = GetFileAttributesW(to);
    if (fAttr != INVALID_FILE_ATTRIBUTES) {
        wchar_t option[5] = {0};
        wprintf(L"A file with the same name exists in the destination directory:\n%ls\nDo you wish to replace "
                " the file or do nothing? [yes/no]: ", to);
        fgetws(option, 5, stdin);
        while (wcscmp_c(option, L"NO") != 0 || wcscmp_c(option, L"YES") != 0) {
            wprintf(L"Please only input 'yes' or 'no': ");
            fgetws(option, 5, stdin);
            wcsupr_c(option);
        }
        if (wcscmp_c(option, L"NO") == 0) {
            return 0;
        }
        if (DeleteFileW(to) == 0) {
            fwprintf(stderr, L"Could not remove file.\n");
            return -1;
        }
    }
    if (MoveFileW(from, to) == 0) {
        fwprintf(stderr, L"Could not move file to target directory.\n");
        return -1;
    }
    wprintf(L"Moved file %ls to target directory. New file path: %ls\n", from, to);
    return 0;
}

int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);

    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argv == NULL) {
        fwprintf(stderr, L"Could not obtain wide-character command-line arguments.\n");
        return -1;
    }

    if (argc == 5) {

    }

    if (argc == 1 || argc == 2) {
        bool all;
        if (argc == 1) {
            all = false;
        }
        else {
            if (wcscmp_c(*(argv + 1), L"-all") != 0) {
                fwprintf(stderr, L"For copying to and from configured directories, the second command-line argument,\n"
                                  "if specified, can only be the flag '-all'.\n"
                                  "Leave blank to copy only most recent file.");
                LocalFree(argv);
                return -1;
            }
            all = true;
        }
        LocalFree(argv);
        wchar_t homePath[PATH_MAX];
        if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, homePath) != S_OK) {
            fwprintf(stderr, L"Error retrieving home path.\n");
            return -1;
        }
        wchar_t *datPath = malloc((wcslen_c(homePath) + 14)*sizeof(wchar_t));
        wcscpy_c(datPath, homePath);
        wcscat_c(datPath, prePath);
        DWORD datAttr = GetFileAttributesW(datPath);
        if (datAttr == INVALID_FILE_ATTRIBUTES) {
            fwprintf(stderr, L"Configuration file not found. "
                             L"Use the -config flag to set source and destination folders.\n");
            free(datPath);
            return -1;
        }
        WIN32_FIND_DATAW fData = {};
        if (FindFirstFileW(datPath, &fData) == INVALID_HANDLE_VALUE) {
            fwprintf(stderr, L"Configuration file information could not be retrieved.\n");
            free(datPath);
            return -1;
        }
        size_t fileSize = MERGE_DWORDS(fData.nFileSizeHigh, fData.nFileSizeLow);
        if (fileSize < 8) {
            fwprintf(stderr, L"Insufficient configuration file file size.\n");
            free(datPath);
            return -1;
        }
        FILE *fp = _wfopen(datPath, L"rb+");
        if (fp == NULL) {
            fwprintf(stderr, L"Configuration file could not be accessed.\n");
            free(datPath);
            return -1;
        }
        size_t count = 0;
        wchar_t *srcDir = malloc(16*sizeof(wchar_t)); wchar_t *s = srcDir;
        wchar_t *dstDir = malloc(16*sizeof(wchar_t)); wchar_t *d = dstDir;
        wchar_t *ext = malloc(8*sizeof(wchar_t)); wchar_t *e = ext;
        while ((*s++ = fgetwc(fp)) && count <= fileSize) {
            if (count % 16 == 0) {
                srcDir = realloc(srcDir, (count + 16)*sizeof(wchar_t));
            }
            ++count;
        }
        while ((*d++ = fgetwc(fp)) && count <= fileSize) {
            if (count % 16 == 0) {
                dstDir = realloc(dstDir, (count + 16)*sizeof(wchar_t));
            }
            ++count;
        }
        while ((*e++ = fgetwc(fp)) && count <= fileSize) {
            if (count % 8 == 0) {
                ext = realloc(ext, (count + 8)*sizeof(wchar_t));
            }
            ++count;
        }
        if (*ext != L'.') {
            fwprintf(stderr, L"Extension in config. file must begin with a full stop '.'.\n");
            return -1;
        }
        DWORD srcAttr = GetFileAttributesW(srcDir);
        DWORD dstAttr = GetFileAttributesW(dstDir);
        if (srcAttr == INVALID_FILE_ATTRIBUTES) {
            fwprintf(stderr, L"Source directory in config. file is not a valid file.\n");
            return -1;
        }
        if (dstAttr == INVALID_FILE_ATTRIBUTES) {
            fwprintf(stderr, L"Destination directory in config. file is not a valid file.\n");
            return -1;
        }
        if (!ISDIR(srcAttr)) {
            fwprintf(stderr, L"Source directory is actually a file.\n");
            return -1;
        }
        if (!ISDIR(dstAttr)) {
            fwprintf(stderr, L"Destination directory is actually a file.\n");
            return -1;
        }
        if (wcscmp_c(ext, L"all") != 0) {
            srcDir = realloc(srcDir, (wcslen_c(srcDir) + wcslen_c(ext) + 3)*sizeof(wchar_t));
            wcscpy_c(srcDir, L"\\*");
            wcscpy_c(srcDir, ext);
        }
        else {
            srcDir = realloc(srcDir, (wcslen_c(srcDir) + 3)*sizeof(wchar_t));
            wcscpy_c(srcDir, L"\\*");
        }
        WIN32_FIND_DATAW srcData = {};
        HANDLE hFind = FindFirstFileW(srcDir, &srcData);
        if (hFind == INVALID_HANDLE_VALUE) {
            fwprintf(stderr, L"File handle to first file in source directory could not be obtained.\n");
            return -1;
        }
        BOOL next = TRUE;
        wchar_t *final = malloc(32767*sizeof(wchar_t));
        wchar_t *finTo = malloc(32767*sizeof(wchar_t));
        DWORD fAttr;
        if (!all) {
            FILETIME fTime = {};
            while (next != 0) {
                if (MERGE_DWORDS(srcData.ftLastWriteTime.dwHighDateTime, srcData.ftLastWriteTime.dwLowDateTime) >
                    MERGE_DWORDS(fTime.dwHighDateTime, fTime.dwLowDateTime) && !ISDIR(srcData.dwFileAttributes)) {
                    fTime.dwHighDateTime = srcData.ftLastWriteTime.dwHighDateTime;
                    fTime.dwLowDateTime = srcData.ftLastWriteTime.dwLowDateTime;
                    wcscpy_c(final, L"\\\\?\\"); wcscat_c(final, srcDir); wcscat_c(final, L"\\");
                    wcscpy_c(final, srcData.cFileName);
                    wcscpy_c(finTo, L"\\\\?\\"); wcscat_c(finTo, dstDir); wcscat_c(finTo, L"\\");
                    wcscpy_c(finTo, srcData.cFileName);
                }
                next = FindNextFileW(hFind, &srcData);
            }
            return file_exists(final, finTo);
        }
        while (next != 0) {
            if (!ISDIR(srcData.dwFileAttributes)) {
                wcscpy_c(final, L"\\\\?\\");
                wcscpy_c(final, srcDir);
                wcscpy_c(final, srcData.cFileName);
                wcscpy_c(finTo, L"\\\\?\\");
                wcscpy_c(finTo, dstDir);
                wcscpy_c(finTo, srcData.cFileName);
                if (file_exists(final, finTo) == -1) {
                    return -1;
                }
            }
            next = FindNextFileW(hFind, &srcData);
        }
        return 0;
    }

    wchar_t *src = *(argv + 1);
    wchar_t *dst = *(argv + 2);

    DWORD srcAttr = GetFileAttributesW(src);



    //if (S_ISREG(buff1.st_mode) && S_ISDIR(buff2.st_mode)) {

    //}
    LocalFree(argv);
    return 0;
}
#endif