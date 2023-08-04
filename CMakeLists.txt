cmake_minimum_required(VERSION 3.20)
project(arpctl LANGUAGES CXX)

file(GLOB_RECURSE SRC src/*.cc)

add_executable(arpctl ${SRC})
target_link_libraries(arpctl PRIVATE pcap tbb)
target_include_directories(arpctl PRIVATE inc)
target_compile_features(arpctl PRIVATE cxx_std_23)
target_compile_options(arpctl PRIVATE -Wall -Wextra -fno-rtti)
target_precompile_headers(arpctl PRIVATE inc/pch.h)
