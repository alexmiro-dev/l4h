cmake_minimum_required(VERSION 3.22.1)
project(l4h)

#================================================================================
# Preamble
#================================================================================
set(CMAKE_VERBOSE_MAKEFILE      true)
set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_BUILD_TYPE            Debug)

add_compile_options(-fconcepts-diagnostics-depth=3 -fcoroutines)

#================================================================================
# Modules
#================================================================================

add_subdirectory(src)
