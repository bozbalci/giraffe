cmake_minimum_required(VERSION 3.13)
project(hw1)

set(CMAKE_CXX_STANDARD 17)


file(GLOB SOURCE_COMMON src/*.cpp)
file(GLOB HEADER_COMMON src/*.h)

add_executable(rasterizer
        ${SOURCE_COMMON}
        ${HEADER_COMMON})
