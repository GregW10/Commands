//
// Created by Gregor Hartl Watters on 05/10/2022.
//

#include "vecs.h"

int main(int argc, char **argv) {
    if (argc > 8 || argc < 7) {
        fprintf(stderr, "Invalid number of arguments (7-8 expected, including exec. name).\n"
                        "Specify the components of the first and second vectors in x y z order, and separate with "
                        "spaces.\nOptionally, specify at any point the decimal places desired with \"dp=...\", where "
                        "... is some integer number (up to 255).\n");
        return -1;
    }
    bool is_fp = false;
    bool has_dp = false;
    unsigned char dp = 10;
    unsigned char dp_index;
    process_args(&dp, &dp_index, &is_fp, &has_dp, argc, argv);
    if (is_fp) {
        vector_ld v1;
        vector_ld v2;
        if (has_dp) {
            v1.x = strtold(*(argv + (1 < dp_index ? 1 : 2)), NULL);
            v1.y = strtold(*(argv + (2 < dp_index ? 2 : 3)), NULL);
            v1.z = strtold(*(argv + (3 < dp_index ? 3 : 4)), NULL);
            v2.x = strtold(*(argv + (4 < dp_index ? 4 : 5)), NULL);
            v2.y = strtold(*(argv + (5 < dp_index ? 5 : 6)), NULL);
            v2.z = strtold(*(argv + (6 < dp_index ? 6 : 7)), NULL);
        }
        else {
            v1.x = strtold(*(argv + 1), NULL);
            v1.y = strtold(*(argv + 2), NULL);
            v1.z = strtold(*(argv + 3), NULL);
            v2.x = strtold(*(argv + 4), NULL);
            v2.y = strtold(*(argv + 5), NULL);
            v2.z = strtold(*(argv + 6), NULL);
        }
        putchar('(');
        print_vec_ld(&v1, dp, false);
        printf(") * (");
        print_vec_ld(&v2, dp, false);
        char format[13];
        sprintf(format, ") = %%.%dLf\n", dp);
        printf(format, dot(&v1, &v2));
        return 0;
    }
    vector_ll v1;
    vector_ll v2;
    if (has_dp) {
        v1.x = strtoll(*(argv + (1 < dp_index ? 1 : 2)), NULL, 10);
        v1.y = strtoll(*(argv + (2 < dp_index ? 2 : 3)), NULL, 10);
        v1.z = strtoll(*(argv + (3 < dp_index ? 3 : 4)), NULL, 10);
        v2.x = strtoll(*(argv + (4 < dp_index ? 4 : 5)), NULL, 10);
        v2.y = strtoll(*(argv + (5 < dp_index ? 5 : 6)), NULL, 10);
        v2.z = strtoll(*(argv + (6 < dp_index ? 6 : 7)), NULL, 10);
    }
    else {
        v1.x = strtoll(*(argv + 1), NULL, 10);
        v1.y = strtoll(*(argv + 2), NULL, 10);
        v1.z = strtoll(*(argv + 3), NULL, 10);
        v2.x = strtoll(*(argv + 4), NULL, 10);
        v2.y = strtoll(*(argv + 5), NULL, 10);
        v2.z = strtoll(*(argv + 6), NULL, 10);
    }
    putchar('(');
    print_vec_ll(&v1, false);
    printf(") * (");
    print_vec_ll(&v2, false);
    printf(") = %lld\n", dot(&v1, &v2));
    return 0;
}
