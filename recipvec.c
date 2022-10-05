//
// Created by Gregor Hartl Watters on 04/10/2022.
//

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "vecs.h"

static inline vector_ld b1(const vector_ld *a1, const vector_ld *a2, const vector_ld *a3) {
    if (a1 == NULL || a2 == NULL || a3 == NULL) {
        fprintf(stderr, "One or more of the vectors passed to the b1() function is NULL.\n");
        abort();
    }
    vector_ld top = cross(a2, a3);
    long double bottom = dot(a1, &top);
    vector_ld fraction = div_vec_s(&top, bottom);
    return mul_s_vec(2*M_PI, &fraction);
}

static inline vector_ld b2(const vector_ld *a1, const vector_ld *a2, const vector_ld *a3) {
    if (a1 == NULL || a2 == NULL || a3 == NULL) {
        fprintf(stderr, "One or more of the vectors passed to the b1() function is NULL.\n");
        abort();
    }
    vector_ld top = cross(a3, a1);
    vector_ld a2_a3 = cross(a2, a3);
    long double bottom = dot(a1, &a2_a3);
    vector_ld fraction = div_vec_s(&top, bottom);
    return mul_s_vec(2*M_PI, &fraction);
}

static inline vector_ld b3(const vector_ld *a1, const vector_ld *a2, const vector_ld *a3) {
    if (a1 == NULL || a2 == NULL || a3 == NULL) {
        fprintf(stderr, "One or more of the vectors passed to the b1() function is NULL.\n");
        abort();
    }
    vector_ld top = cross(a1, a2);
    vector_ld a2_a3 = cross(a2, a3);
    long double bottom = dot(a1, &a2_a3);
    vector_ld fraction = div_vec_s(&top, bottom);
    return mul_s_vec(2*M_PI, &fraction);
}

int main(int argc, char **argv) {

    vector_ld a1;
    vector_ld a2;
    vector_ld a3;

    vector_ld b1_v;
    vector_ld b2_v;
    vector_ld b3_v;

    unsigned char dp = 10;
    unsigned char dp_index;
    bool has_dp = false;

    if (argc <= 2) {
        if (argc == 2) {
            process_args(&dp, &dp_index, NULL, &has_dp, argc, argv);
        }
        printf("Enter the x, y and z components separated by a space.\n");
        printf("Enter a1 primitive vector:\n");
        get_vec(&a1);
        printf("Enter a2 primitive vector:\n");
        get_vec(&a2);
        printf("Enter a3 primitive vector:\n");
        get_vec(&a3);
        goto deal;
    }
    else if (argc == 10 || argc == 11) {
        process_args(&dp, &dp_index, NULL, &has_dp, argc, argv);
        if (has_dp) {
            a1.x = strtold(*(argv + (1 < dp_index ? 1 : 2)), NULL);
            a1.y = strtold(*(argv + (2 < dp_index ? 2 : 3)), NULL);
            a1.z = strtold(*(argv + (3 < dp_index ? 3 : 4)), NULL);
            a2.x = strtold(*(argv + (4 < dp_index ? 4 : 5)), NULL);
            a2.y = strtold(*(argv + (5 < dp_index ? 5 : 6)), NULL);
            a2.z = strtold(*(argv + (6 < dp_index ? 6 : 7)), NULL);
            a3.x = strtold(*(argv + (7 < dp_index ? 4 : 8)), NULL);
            a3.y = strtold(*(argv + (8 < dp_index ? 5 : 9)), NULL);
            a3.z = strtold(*(argv + (9 < dp_index ? 6 : 10)), NULL);
        }
        else {
            a1.x = strtold(*(argv + 1), NULL);
            a1.y = strtold(*(argv + 2), NULL);
            a1.z = strtold(*(argv + 3), NULL);
            a2.x = strtold(*(argv + 4), NULL);
            a2.y = strtold(*(argv + 5), NULL);
            a2.z = strtold(*(argv + 6), NULL);
            a3.x = strtold(*(argv + 7), NULL);
            a3.y = strtold(*(argv + 8), NULL);
            a3.z = strtold(*(argv + 9), NULL);
        }
        deal:
        b1_v = b1(&a1, &a2, &a3);
        printf("b1: ");
        print_vec_ld(&b1_v, dp, true);
        b2_v = b2(&a1, &a2, &a3);
        printf("b2: ");
        print_vec_ld(&b2_v, dp, true);
        b3_v = b3(&a1, &a2, &a3);
        printf("b3: ");
        print_vec_ld(&b3_v, dp, true);
        return 0;
    }
    fprintf(stderr, "Invalid number of command-line arguments.\n"
                    "To enter the vectors a1, a2 and a3, either run the program without specifying any additional "
                    "command-line arguments,\nor enter each component for each vector as separate command-line "
                    "arguments (in x,y,z order).\nAn optional \"dp=...\" is always permitted for specifying the "
                    "number of decimal places.\n");
    return -1;
}
