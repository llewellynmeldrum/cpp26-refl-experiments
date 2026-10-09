cmake_minimum_required(VERSION 4.3.3)

set(APP_NAME cpp26-refl)
project(${APP_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Tell CMake to look inside the 'cmake' folder for module files
list(APPEND CMAKE_MODULE_PATH 
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)



file( GLOB_RECURSE APP_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${APP_NAME} 
    "${APP_SRC}"
)
target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


target_compile_options(${APP_NAME} PRIVATE
    -Wall
    -Wswitch                # Force errors like 'enum value x not evaluated in switch statement'
    -Werror                 
    -Wimplicit-fallthrough  # Why is this not part of -Wall
    -Wno-unused             # I cant stand this shit spamming my screen
    -Wunused-result         # These ones are okay though
    -Wfatal-errors         # Gets annoying with clangd (i could disable it in .clangd but im lazy)
    -fdiagnostics-color=always
    -ffile-prefix-map=${CMAKE_SOURCE_DIR}=. # gives stack traces a relative path to cwd
)

set_target_properties( ${APP_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

# TEMPORARY FOR BENCHMARKING COMPILE TIME
#target_compile_options(${APP_NAME} PRIVATE -ftime-report)

# enable reflection only for files with the .refl.cpp extension
file(GLOB_RECURSE REFL_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.refl.cpp")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_source_files_properties(${REFL_SRC} PROPERTIES COMPILE_OPTIONS -freflection)
endif()

# =============================================================================
# LIBRARIES
# =============================================================================
include(FetchContent)


# GLM (openGL Mathematics)
# ---------------------------------------
target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/OpenGL-Mathematics"
)
target_compile_definitions(${APP_NAME}
    PUBLIC GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
    PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE
)

# CPPTRACE
# ---------------------------------------
if (ENABLE_CPPTRACE)
    target_include_directories(${APP_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/external/cpptrace"
    )
    set(CPPTRACE_BUILD_SHARED   OFF)   # static ( no runtime .dylib bullshit)
    set(CPPTRACE_BUILD_TESTING  OFF)
    FetchContent_Declare(cpptrace   SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/cpptrace)
    FetchContent_MakeAvailable(cpptrace)
    target_link_libraries(${APP_NAME} PRIVATE cpptrace::cpptrace)
endif()

# LIBASSERT
# ---------------------------------------
target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/libassert/include"
)
add_subdirectory(external/libassert)
target_link_libraries(${APP_NAME} PRIVATE
    libassert::assert
)


# MAGIC ENUM
# ---------------------------------------
target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/magic_enum"
)
set(MAGIC_ENUM_OPT_BUILD_TESTS OFF)
set(MAGIC_ENUM_OPT_INSTALL     OFF)
FetchContent_Declare(magic_enum SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/magic_enum)


# REFLECT CPP
# ---------------------------------------
target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/reflect-cpp"
)
FetchContent_Declare(reflect_cpp SOURCE_DIR 
    "${CMAKE_SOURCE_DIR}/external/reflect-cpp"
)
FetchContent_MakeAvailable(reflect_cpp)
target_link_libraries(${APP_NAME} PRIVATE
    reflectcpp::reflectcpp
)


# RANGE-V3 [HEADER ONLY]
# ---------------------------------------
target_include_directories(${APP_NAME} SYSTEM PRIVATE 
    "${CMAKE_SOURCE_DIR}/external/range-v3/include/"
)
# toml++ [HEADER ONLY]
# ---------------------------------------
target_compile_definitions(${APP_NAME}
    PUBLIC TOML_HEADER_ONLY 
    # we use the STB style #define IMPLEMENTATION style
)
target_include_directories(${APP_NAME} SYSTEM PRIVATE 
    "${CMAKE_SOURCE_DIR}/external/toml++/include"
)

# expected-FS
# ---------------------------------------
target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/expected-fs/include"
)
FetchContent_Declare(expected_fs SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/external/expected-fs"
)
FetchContent_MakeAvailable(expected_fs)
target_link_libraries(${APP_NAME} PRIVATE expected_fs::expected_fs)




# --- Optimization and sanitizers
OPTION(_ENABLE_ASAN  "ENABLE ADDRESSSANITIZER" OFF)
OPTION(_ENABLE_LSAN  "ENABLE ADDRESSSANITIZER LEAK DETECTION" OFF)
OPTION(_ENABLE_TSAN  "ENABLE THREADSANITIZER" OFF)
OPTION(_ENABLE_UBSAN "ENABLE UBSAN" OFF)
OPTION(_FAST_MATH    "ENABLE -FFAST-MATH" OFF)
OPTION(_O1 "ENABLE -O1" OFF)
OPTION(_O2 "ENABLE -O2" OFF)
OPTION(_O3 "ENABLE -O3" OFF)

if(_O1)
    target_compile_options(${APP_NAME} PRIVATE -O1)
endif()

if(_O2)
    target_compile_options(${APP_NAME} PRIVATE -O2)
endif()

if(_O3)
    target_compile_options(${APP_NAME} PRIVATE -O3)
endif()

if(_FAST_MATH)
    target_compile_options(${APP_NAME} PRIVATE -ffast-math)
endif()

if(_ENABLE_ASAN OR _ENABLE_LSAN)
    target_compile_options(${APP_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
    target_link_options(${APP_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
endif()

if(_ENABLE_TSAN)
    target_compile_options(${APP_NAME} PRIVATE -fsanitize=thread -fno-omit-frame-pointer -g)
    target_link_options(${APP_NAME} PRIVATE -fsanitize=thread -fno-omit-frame-pointer -g)
endif()

if(_ENABLE_UBSAN)
    target_compile_options(${APP_NAME} PRIVATE -fsanitize=undefined -fno-omit-frame-pointer -g)
    target_link_options(${APP_NAME} PRIVATE -fsanitize=undefined -fno-omit-frame-pointer -g)
endif()


add_custom_target(run
    COMMAND sh -c "./bin/${APP_NAME}"
    DEPENDS ${APP_NAME}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_custom_target(debug
    COMMAND lldb -o run -- "$<TARGET_FILE:${APP_NAME}>"
    DEPENDS ${APP_NAME}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    USES_TERMINAL
)

if(APPLE)
    if(ENABLE_CPPTRACE)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND dsymutil $<TARGET_FILE:${APP_NAME}>
            COMMENT "Generating .dSYM for stack traces for cpptrace/libassert"
        )
    endif()
endif()


