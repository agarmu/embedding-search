cmake_minimum_required(VERSION 3.14)
project(Embedding CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# High-performance flags
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -march=native -flto)
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
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto" CACHE STRING "" FORCE)

# Make available
FetchContent_MakeAvailable(z3)

# Target
add_executable(embedding main.cpp)
target_link_libraries(embedding PRIVATE z3::z3 z3::z3_api)
target_include_directories(embedding PRIVATE ${z3_SOURCE_DIR}/src/api ${z3_BINARY_DIR})
