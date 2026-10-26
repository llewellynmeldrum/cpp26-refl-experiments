# A stand-in for "some other project of yours" that uses the library.
# This is exactly what a real consumer's CMakeLists.txt looks like.
cmake_minimum_required(VERSION 3.31)
project(mylib_test_package LANGUAGES CXX)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CONSUME "find_package" CACHE STRING "How to get mylib: find_package | fetchcontent")

if(CONSUME STREQUAL "find_package")
    # Searches CMAKE_PREFIX_PATH for <prefix>/{share,lib}/cmake/mylib/mylibConfig.cmake.
    # `make package-test` points CMAKE_PREFIX_PATH at a staged install.
    find_package(mylib REQUIRED)
elseif(CONSUME STREQUAL "fetchcontent")
    # Normally downloads the tagged source and builds it as part of this project.
    # `make fetch-test` sets FETCHCONTENT_SOURCE_DIR_MYLIB to the local checkout,
    # so nothing is downloaded and the repository below is never contacted.
    include(FetchContent)
    FetchContent_Declare(mylib
        GIT_REPOSITORY https://github.com/llewellynmeldrum/mylib
        GIT_TAG        v0.1.0
    )
    FetchContent_MakeAvailable(mylib)
else()
    message(FATAL_ERROR "CONSUME must be find_package or fetchcontent, got '${CONSUME}'")
endif()

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE mylib::mylib)
