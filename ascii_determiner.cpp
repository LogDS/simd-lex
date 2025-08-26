//
// Created by gyankos on 25/08/25.
//

#include <assert.h>
#include <bitset>
#include <immintrin.h>

#include <iostream>
#include <limits>
#include <map>
#include <vector>
#include <unordered_set>

uint64_t bitmask (unsigned int n) {
    return _bzhi_u64(~(uint64_t)0, n);
}

template <typename Item> std::tuple<bool, std::vector<Item>, std::vector<Item>, bool, size_t, size_t, size_t> vectorize_Items(const std::unordered_set<Item>& S, bool test = false) {
    // This code provides the initialization of the HIGH and LOW bit maps
    // Differently from the student's code, this is now arbitrary for any item size.

    //This code works over the assumption that there is only one element with the same value of the high and low maks,
    //as per definition of the hashing: in fact, each bit is unique, and we take just its upper and lower element.
    //Thus, we need two bit sets to correctly determine whether it belongs to the set S of expected characters

    // Determining the maximum representable size in terms of bits per one single element
    const size_t max_object_size = std::numeric_limits<Item>::digits;
    // Determining how to split up the mask according to the two halfs of the code
    const size_t half_digits = max_object_size/2;
    // In this code, I am assuming objects being of "pari" size
    assert((max_object_size % 2) == 0);
    // Determining the low bit mask
    auto lowmask = bitmask(half_digits);
    // Determining the high bit mask
    auto highmask = lowmask << half_digits;
    std::map<size_t, std::vector<Item>> low_table, high_table;
    // Considering only the elements considered in the set of interest
    for (const auto& i : S) {
        auto low = ((size_t)i) & lowmask;
        auto high = (((size_t)i) & highmask) >> 4;
        low_table[low].push_back(i);
        high_table[high].push_back(i);
    }

    // Determining which table will have to be the dominant one
    size_t maxLowCount = 0, maxHighCount = 0;
    for (const auto& [k,v] : low_table) {
        auto N = v.size();
        if (N > maxLowCount)
            maxLowCount = N;
    }
    for (const auto& [k,v] : high_table) {
        auto N = v.size();
        if (N > maxHighCount)
            maxHighCount = N;
    }
    std::map<size_t, std::vector<Item>> *target_table, *other_table;
    bool isLowDominant = false;
    if ((maxHighCount <= maxLowCount) and (maxLowCount <= max_object_size)) {
        target_table = &low_table;
        other_table = &high_table;
    } else if (maxLowCount <= max_object_size) {
        target_table = &high_table;
        other_table = &low_table;
        isLowDominant = true;
    } else {
        return {false, {}, {}, false, 0, 0, 0}; // Cannot use the algorithm, as at least one table has to have as many elements as the other column
    }
    // LEGACY:
    // if (maxHighCount <= max_object_size) {
    //     target_table = &low_table;
    //     other_table = &high_table;
    // } else if (maxLowCount <= max_object_size) {
    //     target_table = &high_table;
    //     other_table = &low_table;
    //     isLowDominant = true;
    // } else {
    //     return {false, {}, {}, false, 0, 0, 0}; // Cannot use the algorithm, as at least one table has to have as many elements as the other column
    // }

    // The map with the higher number of elements cannot use all of its bits for the elements being contained:
    // thus, I am just representing the cell with the index associated to the row if any, and with a zero if this
    // contains no elements. In order to compress the information a bit more, we associate incrementally to each
    // element being in the map to an integer
    std::map<size_t, size_t> mapForElement, final_target_table;
    for (const auto& [k,v] : *target_table) {
        final_target_table.emplace(k, (size_t)final_target_table.size());
    }
    // The other map has each key associated with the number of elements from the other map, by setting up the flags
    // with the correspondent elements being active
    for (const auto& [k, v]: *other_table) {
        if (test)
            std::cout << k << std::endl;
        std::bitset<max_object_size> bs(0);
        for (const auto& x: v) {
            assert(S.contains(x));
            if (isLowDominant) {
                auto high = (x & highmask) >> half_digits;
                auto it = final_target_table.find(high);
                assert(it != final_target_table.end());
                bs.set(it->second);
            } else {
                auto low = (x & lowmask);
                auto it = final_target_table.find(low);
                assert(it != final_target_table.end());
                bs.set(it->second);
            }
        }
        if (bs.any()) {
            mapForElement[k] = bs.to_ullong();
            if (test)
                std::cout << bs <<  " = " << bs.to_ullong() << std::endl ;
        }
    }

    // This is mainly required for the run time execution of the algorithm: determining the low and high map for the values,
    // according to the two previously defined maps.
    std::vector<Item> HIGH(lowmask+1, 0), LOW(lowmask+1, 0);
    for (size_t i = 0, N = lowmask+1; i<N; i++) {
        size_t fmV = 0;
        auto it = final_target_table.find(i);
        if (it != final_target_table.end()) {
            std::bitset<max_object_size> fromMap;
            fromMap.set(it->second);
            fmV = fromMap.to_ullong();;
        }
        size_t mfe = 0;
        auto it2 = mapForElement.find(i);
        if (it2 != mapForElement.end()) {
            mfe = it2->second;
        }
        if (isLowDominant) {
            HIGH[i] = fmV;
            LOW[i] = mfe;
        } else {
            LOW[i] = fmV;
            HIGH[i] = mfe;
        }
    }

    if (test) {
        // TEST
        for (unsigned char i = 0, N = std::numeric_limits<Item>::max(); i < N; ++i) {
            std::cout << i << std::endl;
            unsigned char mask1 = (i & highmask) >> half_digits, mask2 = i & lowmask;
            if (isLowDominant) {
               std::swap(mask1, mask2);
            }
            std::bitset<max_object_size> value(i);
            bool test = false;
            auto it = mapForElement.find(mask1);
            if (i == '(')
                std::cout <<"ERROR" << std::endl;
            if (it != mapForElement.end()) {
                if (!isLowDominant) {
                    assert(HIGH[mask1] == it->second);
                } else {
                    assert(LOW[mask1] == it->second);
                }
                std::bitset<max_object_size> val;
                /*if (mask2 < max_object_size)*/ {
                    auto it2 = final_target_table.find(mask2);
                    std::bitset<max_object_size> fromMap;
                    if (it2 != final_target_table.end()) {
                        unsigned char valMask2 = 0;
                        valMask2 = it2->second;
                        fromMap.set(valMask2);
                    }
                    if (isLowDominant) {
                        assert(HIGH[mask2] ==fromMap.to_ullong());
                    } else {
                        assert(LOW[mask2] == fromMap.to_ullong());
                    }
                    // val.set(mask2);
                    std::cout << ((size_t)i) << " = " << value.to_string() << (isprint(i) ? " yes " : " no ") << (S.contains(i) ? " yes " : " no ") << (std::bitset<8>(it->second).to_string()) << " " << (fromMap.to_string()) << std::endl;
                    test = it->second & (unsigned char)fromMap.to_ullong();
                    assert(test == (S.contains(i)));
                }
            } else {
                if (isLowDominant) {
                    assert(LOW[mask1] == 0);
                } else {
                    assert(HIGH[mask1] == 0);
                }
            }
            assert(test == (S.contains(i)));
        }
    }



    return {true, LOW, HIGH, isLowDominant, lowmask, highmask, half_digits};
}

template <typename Item, bool isLowDominant> bool test(const std::vector<Item>& HIGH,
                                                        const std::vector<Item>& LOW,
                                                        size_t lowmask,
                                                        size_t highmask,
                                                        size_t half_digits,
                                                        const Item& item/*,
                                                        std::unordered_set<Item>& S_debugging*/) {
    //bool debug = S_debugging.contains(item);
    // Mimicking the vectorization test

    // Getting the hash values for each vector
    unsigned char mask_H = (item &highmask) >> half_digits, mask_L = item & lowmask;
    // Computing the test by bit operations: the element belongs to the map if the vector containing less items has the flag corresponding to the vector containing more.
    auto result = (bool)(HIGH[mask_H] & LOW[mask_L]);
    // std::cout << ((size_t)item) << " = " << (std::bitset<8>(item).to_string())<< (isprint(item) ? " yes " : " no ") << (S_debugging.contains(item) ? " yes " : " no ") << (std::bitset<8>(HIGH[mask_H]).to_string()) << " " << (std::bitset<8>(LOW[mask_L]).to_string()) << std::endl;
    //assert(result == debug);
    return result;

    // // if (isLowDominant) {
    // //    std::swap(mask1, mask2);
    // // }
    // // std::bitset<max_object_size> value(i);
    // // std::cout << ((size_t)i) << " = " << value.to_string() << (isprint(i) ? " yes " : " no ") << (S.contains(i) ? " yes " : " no ") << (std::bitset<8>(mask1).to_string()) << std::endl;
    // bool test = false;
    // auto it = mapForElement.find(mask1);
    // if (it != mapForElement.end()) {
    //     std::bitset<max_object_size> val;
    //     if (mask2 < max_object_size) {
    //         auto it2 = straightTable.find(mask2);
    //         std::bitset<max_object_size> fromMap;
    //         if (it2 != straightTable.end()) {
    //             unsigned char valMask2 = 0;
    //             valMask2 = it2->second;
    //             fromMap.set(valMask2);
    //         }
    //         // val.set(mask2);
    //         test = it->second & (unsigned char)fromMap.to_ullong();
    //     }
    // }
    // assert(test == (S.contains(i)));
}


int main(void) {
    std::unordered_set<unsigned char> S{'(',')','[',']','{','}',',',';','+','-','~','%','<','>','?','!','*','^','&','=','.','|','/',':'};
    auto vectorization = vectorize_Items(S);
    // assert(std::get<3>(vectorization));

    //
    // const size_t max_object_size = 8;
    //
    // std::map<int, std::vector<unsigned char>> low_table, high_table;
    // for (unsigned char i = 0, N = std::numeric_limits<unsigned char>::max(); i < N; ++i) {
    //     if (S.contains(i)) {
    //         auto low = i & 0x0F;
    //         auto high = (i & 0xF0) >> 4;
    //         low_table[low].push_back(i);
    //         high_table[high].push_back(i);
    //     }
    // }
    //
    //
    // size_t maxLowCount = 0, maxHighCount = 0;
    // for (const auto& [k,v] : low_table) {
    //     auto N = v.size();
    //     if (N > maxLowCount)
    //         maxLowCount = N;
    // }
    // for (const auto& [k,v] : high_table) {
    //     auto N = v.size();
    //     if (N > maxHighCount)
    //         maxHighCount = N;
    // }
    //
    // std::map<int, std::vector<unsigned char>> *target_table, *other_table;
    // bool isLowDominant = false;
    // if (maxHighCount <= max_object_size) {
    //     target_table = &low_table;
    //     other_table = &high_table;
    // } else if (maxLowCount <= max_object_size) {
    //     target_table = &high_table;
    //     other_table = &low_table;
    //     isLowDominant = true;
    // } else {
    //     return 1; // Cannot use the algorithm, as at least one table has to have as many elements as the other column
    // }
    //
    // std::map<int, size_t> mapForElement, straightTable;
    // for (const auto& [k,v] : *target_table) {
    //     straightTable.emplace(k, (size_t)straightTable.size());
    // }
    // for (const auto& [k, v]: *other_table) {
    //     std::cout << k << std::endl;
    //     std::bitset<max_object_size> bs(0);
    //     for (const auto& v: v) {
    //         assert(S.contains(v));
    //         // if (!S.contains(v))
    //         //     continue;
    //         if (isLowDominant) {
    //             auto high = (v & 0xF0) >> 4;
    //             auto it = straightTable.find(high);
    //             assert(it != straightTable.end());
    //             bs.set(it->second);
    //         } else {
    //             auto low = (v & 0x0F);
    //             auto it = straightTable.find(low);
    //             assert(it != straightTable.end());
    //             bs.set(it->second);
    //         }
    //     }
    //     if (bs.any()) {
    //         mapForElement[k] = bs.to_ullong();
    //         std::cout << bs <<  " = " << bs.to_ullong() << std::endl ;
    //     }
    // }

    std::cout << "hello worlds" << std::endl;
    for (unsigned char i = 0, N = std::numeric_limits<unsigned char>::max(); i < N; ++i) {
        test<unsigned char, true>(std::get<2>(vectorization), std::get<1>(vectorization), std::get<4>(vectorization), std::get<5>(vectorization), std::get<6>(vectorization), i/*, S*/);
    }


}