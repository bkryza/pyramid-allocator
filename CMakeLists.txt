cmake_minimum_required(VERSION 3.22)
project(pyramid_allocator)

set(CMAKE_CXX_STANDARD 17)

include(cmake/CPM.cmake)

set(BUILD_ORM OFF)

CPMAddPackage(
        NAME benchmark
        VERSION 1.7.1
        GITHUB_REPOSITORY google/benchmark
        GIT_TAG v1.7.1
        OPTIONS "BENCHMARK_ENABLE_TESTING OFF" "BENCHMARK_ENABLE_INSTALL OFF" "BENCHMARK_INSTALL_DOCS OFF"
)

include_directories("include")

add_subdirectory(benchmarks)

add_executable(pyramid_allocator main.cc)
