cmake_minimum_required(VERSION 3.14)
project(bej_parser LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ENABLE_TESTS "Build tests" ON)
option(USE_ARENA_ALLOCATOR "Enable arena allocator (optional)" OFF)
option(OPTIMIZE_SIZE "Compile with -Os for size" ON)


if(OPTIMIZE_SIZE)
  add_compile_options(-Os -g)
else()
  add_compile_options(-O2 -g)
endif()
add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src)

if(ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.6.1
    )
    FetchContent_MakeAvailable(unity)
    enable_testing()
    add_subdirectory(tests)
endif()