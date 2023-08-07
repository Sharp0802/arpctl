cmake_minimum_required(VERSION 3.20)
project(arpctl LANGUAGES CXX)

cmake_host_system_information(RESULT NPROC QUERY NUMBER_OF_LOGICAL_CORES)
set(CMAKE_BUILD_PARALLEL_LEVEL ${NPROC})

file(GLOB_RECURSE SRC src/*.cc)

add_executable(arpctl ${SRC})
target_link_libraries(arpctl PRIVATE pcap tbb)
target_include_directories(arpctl PRIVATE inc)
target_compile_features(arpctl PRIVATE cxx_std_23)
target_compile_options(arpctl PRIVATE -Wall -Wextra -fno-rtti -march=native -mavx2)
target_precompile_headers(arpctl PRIVATE inc/pch.h)
