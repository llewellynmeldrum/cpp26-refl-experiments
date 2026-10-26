cmake_minimum_required(VERSION 3.31)

# The project name IS the library name, the CMake package name, the include
# directory (include/mylib/) and the target namespace (mylib::mylib).
# scripts/init.sh renames all of them at once.
project(mylib VERSION 0.1.0 LANGUAGES CXX)

# =============================================================================
# KNOBS (set by the library author, not the consumer)
# =============================================================================
# Minimum standard a CONSUMER must compile with. This propagates to them.
set(LIB_CXX_STANDARD 23)

# ON -> consumers are compiled with C++26 and, on GCC, -freflection.
# Needed when public headers use ^^T, [: :], template for, [[=annotations]].
set(LIB_NEEDS_REFLECTION OFF)

if(LIB_NEEDS_REFLECTION)
    set(LIB_CXX_STANDARD 26)
endif()

# Settings that only make sense when this repo is the thing being built,
# never when a consumer pulls it in with add_subdirectory / FetchContent.
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
endif()

# =============================================================================
# THE LIBRARY
# =============================================================================
file(GLOB_RECURSE LIB_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# No .cpp files in src/ -> header-only (INTERFACE) library.
# Any .cpp in src/      -> compiled library (static, or shared with -DBUILD_SHARED_LIBS=ON).
#
# _usage is the keyword for "things consumers inherit":
#   INTERFACE for header-only (nothing is compiled here, everything is for consumers)
#   PUBLIC    for compiled    (used by our .cpp files AND inherited by consumers)
if(LIB_SOURCES)
    add_library(${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PRIVATE ${LIB_SOURCES})
    set(_usage PUBLIC)
else()
    add_library(${PROJECT_NAME} INTERFACE)
    set(_usage INTERFACE)
endif()

# Same spelling for every consumer: add_subdirectory, FetchContent and
# find_package all end up with `mylib::mylib`.
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# A header FILE_SET does two jobs: it adds include/ to the include path while
# building, and tells install() which headers to copy and where consumers find them.
target_sources(${PROJECT_NAME} ${_usage}
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES ${LIB_HEADERS}
)

target_compile_features(${PROJECT_NAME} ${_usage} cxx_std_${LIB_CXX_STANDARD})

if(LIB_NEEDS_REFLECTION)
    target_compile_options(${PROJECT_NAME} ${_usage}
        $<$<CXX_COMPILER_ID:GNU>:-freflection>
    )
endif()

# =============================================================================
# DEPENDENCIES
# =============================================================================
# If a dependency appears in your PUBLIC headers, consumers need it too, so:
#   1. find it here with find_package(),
#   2. link it with ${_usage},
#   3. add a matching find_dependency() to cmake/Config.cmake.in.
#
# FIND_PACKAGE_ARGS makes FetchContent try find_package() first and only
# download when the package is not installed. Best of both for a personal lib.
#
#   include(FetchContent)
#   FetchContent_Declare(toml11
#       GIT_REPOSITORY https://github.com/ToruNiina/toml11
#       GIT_TAG        v4.4.0
#       FIND_PACKAGE_ARGS 4
#   )
#   FetchContent_MakeAvailable(toml11)
#   target_link_libraries(${PROJECT_NAME} ${_usage} toml11::toml11)

# =============================================================================
# DEVELOPMENT: tests, examples, warnings, sanitizers
# =============================================================================
# Defaults ON only when this repo is the top-level project, so consumers never
# build our tests or inherit our -Werror. A consumer can still opt in.
option(${PROJECT_NAME}_BUILD_TESTS    "Build ${PROJECT_NAME} tests"    ${PROJECT_IS_TOP_LEVEL})
option(${PROJECT_NAME}_BUILD_EXAMPLES "Build ${PROJECT_NAME} examples" ${PROJECT_IS_TOP_LEVEL})

if(${PROJECT_NAME}_BUILD_TESTS OR ${PROJECT_NAME}_BUILD_EXAMPLES)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/dev_flags.cmake")
    if(LIB_SOURCES)
        # BUILD_INTERFACE: applies to our own .cpp files, never exported to consumers.
        target_link_libraries(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${PROJECT_NAME}_dev_flags>)
    endif()
endif()

if(${PROJECT_NAME}_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# =============================================================================
# INSTALL (make install / cmake --install)
# =============================================================================
option(${PROJECT_NAME}_INSTALL "Generate install rules for ${PROJECT_NAME}" ${PROJECT_IS_TOP_LEVEL})
if(${PROJECT_NAME}_INSTALL)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/install.cmake")
endif()
