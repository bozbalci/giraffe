cmake_minimum_required(VERSION 3.12.4)
project(hw3)

set(CMAKE_CXX_STANDARD 17)

add_executable(hw3_flat src/flat.cpp src/util.cpp src/util.h src/globals.h)
add_executable(hw3_sphere src/sphere.cpp src/globals.h)
target_compile_options(hw3_flat PRIVATE -Wall -Wextra)
target_compile_options(hw3_sphere PRIVATE -Wall -Wextra)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

if (APPLE)
    # No need for GLEW on Mac OS X
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
    target_link_libraries(hw3_flat glfw ${GLFW_LIBRARIES} ${CMAKE_DL_LIBS})
    target_link_libraries(hw3_sphere glfw ${CMAKE_DL_LIBS})
else ()
    target_link_libraries(hw3_flat glfw GLEW ${CMAKE_DL_LIBS})
    target_link_libraries(hw3_sphere glfw GLEW ${CMAKE_DL_LIBS})
endif ()
