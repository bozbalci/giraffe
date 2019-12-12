cmake_minimum_required(VERSION 3.13)
project(learning)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
file(GLOB SOURCE_COMMON src/*.cpp src/*.c)

add_executable(glstuff ${SOURCE_COMMON})

find_package(glfw3 3.3 REQUIRED)

target_link_libraries(glstuff glfw)