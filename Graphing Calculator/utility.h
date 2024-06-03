#pragma once

#include <cstdlib>
#include <iostream>

template <typename T>
inline T* xmalloc() {
    void* ptr = malloc(sizeof(T));

    if (ptr == nullptr) {
        std::cerr << "[ERROR] malloc() couldn't allocate memory!" << std::endl;
        // TODO: Check how to abort() with SFML
        abort();
    }

    return (T*)ptr;
}