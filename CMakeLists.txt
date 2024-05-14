cmake_minimum_required(VERSION 3.27)
project(areteGL)

add_library(areteGL_properties INTERFACE)
target_compile_features(areteGL_properties INTERFACE cxx_std_20)

find_package(OpenGL REQUIRED OpenGL)
add_subdirectory(3rd-party)

include(cmake/resources.cmake)

add_executable(areteGL
        src/main.cpp
        src/options.cpp
        include/arete/options.hpp)
target_link_libraries(areteGL PRIVATE areteGL_properties glad glfw glm)
target_include_directories(areteGL PRIVATE include)
target_compile_shaders(areteGL resources/shaders)