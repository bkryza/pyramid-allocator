/**

 */

#include <iostream>

#include "include/pyramid_allocator.h"
using alloc_t = pyramid_allocator::allocator<std::uint8_t>;

template <typename T>
std::vector<typename pyramid_allocator::allocator<T>::layer>
    pyramid_allocator::allocator<T>::layers_;

int main()
{
    std::vector<std::uint8_t, alloc_t> v{
        alloc_t{8 * 1024, 32 * 1024 * 1024, 8}};

    v.reserve(2 * 1024);

    std::cout << "Hello, World!" << std::endl;

    return 0;
}
