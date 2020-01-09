cmake_minimum_required(VERSION 3.12.4)
project(hw3)

set(CMAKE_CXX_STANDARD 17)

add_executable(hw3_flat src/flat.cpp)
add_executable(hw3_sphere src/sphere.cpp)
target_compile_options(hw3_flat PRIVATE -Wall -Wextra)
target_compile_options(hw3_sphere PRIVATE -Wall -Wextra)

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(jpeg REQUIRED)

target_link_libraries(hw3_flat ${JPEG_LIBRARIES})
target_link_libraries(hw3_sphere ${JPEG_LIBRARIES})
include_directories(${JPEG_INCLUDE_DIR})

if (APPLE)
    # Silence deprecation warnings on macOS
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")

    # macOS clients does not need to use GLEW
    target_link_libraries(hw3_flat glfw ${GLFW_LIBRARIES} ${CMAKE_DL_LIBS})
    target_link_libraries(hw3_sphere glfw ${GLFW_LIBRARIES} ${CMAKE_DL_LIBS})
else ()
    target_link_libraries(hw3_flat glfw GLEW ${CMAKE_DL_LIBS})
    target_link_libraries(hw3_sphere glfw GLEW ${GLFW_LIBRARIES} ${CMAKE_DL_LIBS})
endif ()
