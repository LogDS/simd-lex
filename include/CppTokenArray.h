//
// Created by gyankos on 24/08/25.
//

#ifndef CPPTOKENARRAY_H
#define CPPTOKENARRAY_H

#include <avxintrin.h>
#include <immintrin.h>
#include <stdint.h>

#include "aligned_vector.hpp"

template <size_t alignment>
struct cppTokenArray {
    aligned_T_vector<uint8_t, alignment> token_types;
    aligned_T_vector<uint32_t, alignment> token_locs;
    char* src;
    size_t size;

    //create_empty_token_array
    cppTokenArray() : src{nullptr}, size{0} {}

    // append_token
    void append_token(uint8_t type, uint32_t log) {
        token_types.emplace_back(type);
        token_locs.emplace_back(log);
        size++;
    }

    void append_tokens(__m256i types, __m256i locs, int size, uint32_t start_idx) {
        _mm256_storeu_si256(
            (__m256i *) (token_types + this->size),
            types
        );

        uint64_t *locs_64 = (uint64_t*) &locs;
        __m256i start_idx_vec = _mm256_set1_epi32(start_idx);

        for (uint8_t i = 0; i < 4 && size > 0; ++i) {
            __m256i locs_expanded = _mm256_cvtepu8_epi32(
                _mm_set_epi64x(0, *(locs_64 + i))   // Set lower 64 bits to current locations
            );

            locs_expanded = _mm256_add_epi32(
                locs_expanded,
                start_idx_vec
            );

            _mm256_storeu_si256(
                (__m256i *) (token_locs + this->size),
                locs_expanded
            );

            this->size += 8;   // Assume that we always read 8 bytes. Adjust size later
            size -= 8;
        }

        this->size += size;    // Adjust size
    }
};

#endif //CPPTOKENARRAY_H
