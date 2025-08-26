#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lexer.h"

bool parse_flags(int argc, char **argv, bool *time_flag) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: simd-lexer <file path> [-t/--time].\n");
        return false;
    }

    *time_flag = false;
    if (argc == 3 && (strcmp(argv[2], "-t") == 0 || strcmp(argv[2], "--time") == 0)) {
        *time_flag = true;
    }

    return true;
}

void print_results(TokenArray tokens, bool time_flag, double avg_time) {
    if (time_flag) {
        printf("Avg. time: %f ms\n", avg_time);
    } else {
        print_tokens(tokens);
    };
}

/* - There is a simpler function, _mm256_permutexvar_epi8, but has more latency.
 *   This was defined to abide by the specifications from VPSHUFB
 *
 */

void test(const uint_fast64_t a[32], const uint_fast64_t b[32], uint_fast64_t r[32]) {
    // uint_fast64_t  i;
    for (uint_fast64_t i = 0; i<32; i++) {
        if (b[i] & 0x80) {
            r[i] = 0;
        } else {
            uint_fast64_t val = b[i] & 0x0F;
            if (i >= 16)
                val += 16;
            r[i] = a[val];
        }
    }
    // for (i = 0; i < 16; i++){
    //     // Iteration over 0..15
    //     if (b[i] & 0x80){
    //         r[i] =  0;              // Put zero if the most significant bit of b[i] is 1 (why?)
    //     } else{
    //         r[i] = a[b[i] & 0x0F];  // Given x the lower 16 bits of b[i], return a[x]
    //     }
    // }
    // for (i = 16; i < 32; i++){
    //     // Iteration over 16..31
    //     if (b[i] & 0x80){
    //         r[i] =  0;            // Put zero if the most significant bit of b[i] is 1 (why?)
    //     } else{
    //         r[i] = a[16+(b[i] & 0x0F)];
    //     }
    // }
}




int main(int argc, char **argv) {

//     __m256i lookup1 = _mm256_set_epi8(
//             31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
//     );
//     __m256i values1 = _mm256_set_epi8(
//         0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
// );
//
//     int a[32] = { 31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
//     int r[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//     int b[32] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
//     test(b,a,r);
//
//     // _mm256_shuffle_epi8(lookup1, lookup1);
//     //0,7,0,6,0,5,0,4,0,3,0,2,0,1,0,0,0
//     //0,15,0,14,0,13,0,12,0,11,0,10,0,9,0,8
//     //1,7,1,6,1,5,1,4,1,3,1,2,1,1,1,0,
//     //1,15,1,14,1,13,1,12,1,11,1,10,1,9,1,8
//
//     // _mm256_shuffle_epi8(lookup1, values1);
//     // 0,8,0,9,0,10,0,11,0,12,0,13,0,14,0,15
//     // 0,1,0,2,0,3,0,4,0,5,0,6,0,7
//     // 1,8,1,9,1,10,1,11,1,12,1,13,1,14,1,15
//     // 1,1,1,2,1,3,1,4,1,5,1,6,1,7
//
//
//     __m256i RESULT = _mm256_shuffle_epi8(lookup1   , values1);

    const int repeat_bench = 10;
    double avg_time = 0;
    bool time_flag;

    if (!parse_flags(argc, argv, &time_flag)) {
        return -1;
    }

    char *file_content;
    TokenArray tokens;

    int cnt = repeat_bench;
    do {
        // Start timer
        clock_t start = clock();

        // Run lexer
        tokens = lex_file(argv[1], &file_content);

        // Stop timer
        clock_t end = clock();
        double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000;
        avg_time += cpu_time_used;

        --cnt;
    } while (time_flag && cnt);

    // Results
    print_results(tokens, time_flag, avg_time / repeat_bench);

    // Clean up
    free(file_content);

    return 0;
}
