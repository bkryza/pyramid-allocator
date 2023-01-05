cmake_minimum_required(VERSION 3.22)


add_executable(basic basic.cc)

target_link_libraries(basic benchmark)