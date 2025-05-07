cmake_minimum_required(VERSION 3.14)
project(Embedding CXX)

set(CMAKE_CXX_STANDARD 2a)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# High-performance flags
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -march=native)
add_link_options(-flto)

# Configure FetchContent
include(FetchContent)
FetchContent_Declare(
  z3
  GIT_REPOSITORY https://github.com/Z3Prover/z3.git
  GIT_TAG master
)

# Configure Z3 build flags
set(Z3_BUILD_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(Z3_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(Z3_BUILD_TEST_EXECUTABLES OFF CACHE BOOL "" FORCE)
set(Z3_BUILD_PYTHON_BINDINGS OFF CACHE BOOL "" FORCE)
set(Z3_BUILD_LIBZ3_SHARED OFF CACHE BOOL "" FORCE)
set(Z3_BUILD_LIBZ3_STATIC ON CACHE BOOL "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native" CACHE STRING "" FORCE)

# Make available
FetchContent_MakeAvailable(z3)

# Target
add_executable(embedding main.cpp)
target_link_libraries(embedding PRIVATE libz3)
target_include_directories(embedding PRIVATE
    ${z3_SOURCE_DIR}/src
    ${z3_SOURCE_DIR}/src/api
    ${z3_SOURCE_DIR}/src/api/c++/
    ${z3_SOURCE_DIR}/src/util
    ${z3_BINARY_DIR}
)
