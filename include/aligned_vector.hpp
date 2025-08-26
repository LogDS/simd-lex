//
// Created by gyankos on 24/08/25.
//

#ifndef ALIGNED_VECTOR_H
#define ALIGNED_VECTOR_H

#include <vector>

template <typename T, size_t YourAlignment>
struct aligned_allocator
{
    // ...

    std::vector<T>::pointer allocate(std::vector<T>::size_type pCount, std::vector<T>::const_pointer = nullptr)
    {
        std::vector<T>::pointer mem = 0;
        if (posix_memalign(&mem, YourAlignment, sizeof(T) * pCount) != 0)
        {
            throw std::bad_alloc(); // or something
        }

        return mem;
    }

    void deallocate(std::vector<T>::pointer pPtr, std::vector<T>::size_type)
    {
        free(pPtr);
    }

    // ...
};

template <typename T, size_t YourAlignment> using aligned_T_vector = std::vector<T, aligned_allocator<T, YourAlignment> > ;

#endif //ALIGNED_VECTOR_H
