#include "../include/pyramid_allocator.h"
#include <benchmark/benchmark.h>

using alloc_t = pyramid_allocator::allocator<std::uint8_t>;

using alloc_char_t = pyramid_allocator::allocator<char>;

template <typename T>
std::vector<typename pyramid_allocator::allocator<T>::layer>
    pyramid_allocator::allocator<T>::layers_;

constexpr auto kLoopIterations = 16UL;

static void bm_std_vector_loop(benchmark::State &state)
{
    for (auto _ : state) {
        std::vector<std::vector<std::uint8_t>> vv;
        for (int i = 0; i < kLoopIterations; i++) {
            std::vector<std::uint8_t> v;
            v.reserve(128 * 1024);
            vv.push_back(std::move(v));
        }
    }
}
BENCHMARK(bm_std_vector_loop);

static void bm_pyramid_vector_loop(benchmark::State &state)
{
    std::vector<std::uint8_t, alloc_t> v{
        alloc_t{8 * 1024, 32 * 1024 * 1024, 8}};

    for (auto _ : state) {
        std::vector<std::vector<std::uint8_t, alloc_t>> vv;
        vv.reserve(kLoopIterations);

        for (int i = 0; i < kLoopIterations; i++) {
            std::vector<std::uint8_t, alloc_t> v{
                alloc_t{8 * 1024, 32 * 1024 * 1024, 8}};
            v.reserve(128 * 1024);
            vv.push_back(std::move(v));
        }
    }
}
BENCHMARK(bm_pyramid_vector_loop);

static void bm_std_string_loop(benchmark::State &state)
{
    for (auto _ : state) {
        std::vector<std::string> vv;
        for (int i = 0; i < kLoopIterations; i++) {
            std::string v;
            v.reserve(128 * 1024);
            vv.push_back(std::move(v));
        }
    }
}
BENCHMARK(bm_std_string_loop);

static void bm_pyramid_string_loop(benchmark::State &state)
{
    using pyramid_string =
        std::basic_string<char, std::char_traits<char>, alloc_char_t>;

    pyramid_string v0{alloc_char_t{8 * 1024, 32 * 1024 * 1024, 8}};

    for (auto _ : state) {
        std::vector<pyramid_string> vv;
        vv.reserve(kLoopIterations);

        for (int i = 0; i < kLoopIterations; i++) {
            pyramid_string v{alloc_char_t{8 * 1024, 32 * 1024 * 1024, 8}};
            v.reserve(128 * 1024);
            vv.push_back(std::move(v));
        }
    }
}
BENCHMARK(bm_pyramid_string_loop);

BENCHMARK_MAIN();