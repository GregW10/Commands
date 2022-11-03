//
// Created by Gregor Hartl Watters on 04/10/2022.
//

#ifndef VECS_H
#define VECS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MIN_VEC_STR_SIZE 64 // at 64 it most likely won't even to have to reallocate

typedef enum {
    false, true
} bool;

typedef struct {
    long double x;
    long double y;
    long double z;
} vector_ld;

typedef struct {
    long long x;
    long long y;
    long long z;
} vector_ll;

size_t strlen_c(const char *str) {
    if (str == NULL || *str == 0)
        return 0;
    size_t count = 0;
    while (*str++) ++count;
    return count;
}

static inline long double abs_ld(long double num) {
    return num < 0 ? -num : num;
}

static inline long long unsigned abs_ll(long long num) {
    return num < 0 ? -num : num;
}

#define ABS(num) _Generic((num), long double: abs_ld, long long: abs_ll)(num)

static inline void print_vec_ld(const vector_ld *v, unsigned char dp, bool newline) {
    char format[8];
    sprintf(format, "%%.%dLf", dp);
    printf(format, v->x);
    printf(v->y < 0 ? "i - " : "i + ");
    printf(format, ABS(v->y));
    printf(v->z < 0 ? "j - " : "j + ");
    printf(format, ABS(v->z));
    printf(newline ? "k\n" : "k");
}

static inline void print_vec_ll(const vector_ll *v, bool newline) {
    printf("%lldi ", v->x);
    printf(v->y < 0 ? "- %lldj " : "+ %lldj ", ABS(v->y));
    printf(v->z < 0 ? "- %lldk%s" : "+ %lldk%s", ABS(v->z), newline ? "\n" : "");
}

static inline long double dot_ld(const vector_ld *v1, const vector_ld *v2) {
    if (v1 == NULL || v2 == NULL) {
        fprintf(stderr, "One or more of the vectors passed to the dot() function is NULL.\n");
        abort();
    }
    return v1->x*v2->x + v1->y*v2->y + v1->z*v2->z;
}

static inline vector_ld cross_ld(const vector_ld *v1, const vector_ld *v2) {
    if (v1 == NULL || v2 == NULL) {
        fprintf(stderr, "One or more of the vectors passed to the cross() function is NULL.\n");
        abort();
    }
    vector_ld v = {0, 0, 0};
    v.x = v1->y*v2->z - v1->z*v2->y;
    v.y = v1->z*v2->x - v1->x*v2->z;
    v.z = v1->x*v2->y - v1->y*v2->x;
    return v;
}

static inline unsigned long long dot_ll(const vector_ll *v1, const vector_ll *v2) {
    if (v1 == NULL || v2 == NULL) {
        fprintf(stderr, "One or more of the vectors passed to the dot() function is NULL.\n");
        abort();
    }
    return v1->x*v2->x + v1->y*v2->y + v1->z*v2->z;
}

static inline vector_ll cross_ll(const vector_ll *v1, const vector_ll *v2) {
    if (v1 == NULL || v2 == NULL) {
        fprintf(stderr, "One or more of the vectors passed to the cross() function is NULL.\n");
        abort();
    }
    vector_ll v = {0, 0, 0};
    v.x = v1->y*v2->z - v1->z*v2->y;
    v.y = v1->z*v2->x - v1->x*v2->z;
    v.z = v1->x*v2->y - v1->y*v2->x;
    return v;
}

#define cross(v1, v2) _Generic((v1), const vector_ld*: cross_ld, const vector_ll*: cross_ll, \
                                           vector_ld*: cross_ld, vector_ll*: cross_ll)(v1, v2)
#define dot(v1, v2) _Generic((v1), const vector_ld*: dot_ld, const vector_ll*: dot_ll, \
                                         vector_ld*: dot_ld, vector_ll*: dot_ll)(v1, v2)

static inline vector_ld mul_s_vec(long double s, const vector_ld *v) {
    if (v == NULL) {
        fprintf(stderr, "The vector_ld passed to the mul_s_vec() function is NULL.\n");
        abort();
    }
    vector_ld ret = {s*v->x, s*v->y, s*v->z};
    return ret;
}

static inline vector_ld div_vec_s(const vector_ld *v, long double s) {
    if (v == NULL) {
        fprintf(stderr, "The vector_ld passed to the mul_s_vec() function is NULL.\n");
        abort();
    }
    vector_ld ret = {v->x/s, v->y/s, v->z/s};
    return ret;
}

static inline bool is_digit(char c) {
    return c >= 48 && c <= 57;
}

void strip_non_numeric(char *str) {
    if (str == NULL || *str == 0) {
        return;
    }
    while (*str) {
        if (!is_digit(*str) && *str != '.' && *str != '-' && *str != '+') {
            *str = 32;
        }
        if (*str == '-' && *(str + 1) == '-') { // delete first '-' if followed by another
            *str = 32;
        }
        if (*str == '+' && *(str + 1) == '+') { // same for '+'
            *str = 32;
        }
        ++str;
    }
}

bool has_num(const char *str) {
    if (str == NULL || *str == 0) {
        return false;
    }
    if (*str == '-') {
        ++str;
    }
    while (*str) {
        if (is_digit(*str)) {
            return true;
        }
        ++str;
    }
    return false;
}

bool is_numeric(const char *str, bool *is_fp) {
    if (str == NULL || *str == 0) {
        return false;
    }
    if (*str == '-') {
        ++str;
        if (*str == 0) {
            return false;
        }
    }
    size_t point_count = 0;
    while (*str) {
        if (!is_digit(*str) && *str != '.')
            return false;
        if (*str++ == '.') {
            ++point_count;
            if (is_fp != NULL) {
                *is_fp = true;
            }
        }
    }
    return point_count <= 1;
}

static inline void get_vec(vector_ld *v) {
    if (v == NULL) {
        return;
    }
    char *vec_str = malloc(sizeof(char)*MIN_VEC_STR_SIZE);
    if (vec_str == NULL) {
        fprintf(stderr, "malloc() error.\n");
        abort();
    }
    char *ptr = vec_str;
    int c;
    size_t count = 1; // start at 1 as have to reserve space for null-terminator
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        *ptr++ = (char) c; // the int is guaranteed to only use 8 bits, so casting is fine
        if (count >= MIN_VEC_STR_SIZE && (count & (count - 1)) == 0) { // greater than or eq. to 64 and power of 2
            vec_str = realloc(vec_str, count << 1);
            ptr = vec_str + count;
        }
        ++count;
    }
    *ptr = 0;
    vec_str = realloc(vec_str, sizeof(char)*(strlen_c(vec_str) + 1)); // so the string just fits
    strip_non_numeric(vec_str);
    if (!has_num(vec_str)) {
        fprintf(stderr, "A wholly non-numeric argument was entered.\n");
        abort();
    }
    char *endptr;
    char *endptr2;
    v->x = strtold(vec_str, &endptr); // guaranteed to be at least one number
    v->y = strtold(endptr, &endptr2); // if only one number, y and z components are zero
    v->z = strtold(endptr2, NULL);
    free(vec_str);
}

void process_args(unsigned char *dp, unsigned char *dp_index, bool *is_fp, bool *has_dp, int argc, char **argv) {
    char *ptr;
    for (unsigned char i = 1; i < argc; ++i) {
        if (!is_numeric(*(argv + i), is_fp)) {
            ptr = *(argv + i);
            if (strlen_c(ptr) < 4 || *ptr++ != 'd' || *ptr++ != 'p' || *ptr++ != '=' || !is_numeric(ptr, NULL)) {
                fprintf(stderr, "Argument: \"%s\" is not numeric nor specifies decimal places "
                                "(as \"dp=...\").\n", *(argv + i));
                exit(1);
            }
            if (*has_dp) {
                fprintf(stderr, "Decimal places cannot be specified more than once (using \"dp=...\").\n");
                exit(1);
            }
            *dp_index = i;
            *has_dp = true;
            ptr = *(argv + i) + 3; // make ptr point to character after '=' in "dp=..."
            if (*ptr == 0) {
                goto out;
            }
            *dp = 0;
            for (unsigned char j = 0; j < 3 && *ptr; ++j) {
                if (*ptr == '.') { // quit loop if decimal point found (can't have a fractional number of d.p.)
                    if (j == 0) {
                        *dp = 10;
                    }
                    goto out;
                }
                *dp *= 10;
                *dp += *ptr++ - 48; // convert argument from ASCII to number
            }
            out:
            continue; // must have expression after goto
        }
    }
}
#endif
