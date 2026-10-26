# =============================================================================
# <name>_dev_flags: warnings, optimisation and sanitizers for code IN THIS REPO.
#
# It is an INTERFACE target that tests, examples and (if compiled) the library's
# own .cpp files link to. It is never installed or exported, so consumers of the
# library never inherit -Werror, -ffast-math or sanitizer flags.
# =============================================================================
set(_dev ${PROJECT_NAME}_dev_flags)
add_library(${_dev} INTERFACE)

target_compile_options(${_dev} INTERFACE
    -Wall
    -Wextra                 # consumers may build with this; keep your headers clean under it
    -Wswitch                # Force errors like 'enum value x not evaluated in switch statement'
    -Wimplicit-fallthrough  # Why is this not part of -Wall
    -Wno-unused             # I cant stand this shit spamming my screen
    -Wunused-result         # These ones are okay though
#   -Wpedantic              # stricter still; trips on GNU extensions such as "\e"
    -fdiagnostics-color=always
    -ffile-prefix-map=${PROJECT_SOURCE_DIR}=. # gives stack traces a relative path to cwd
)

OPTION(_TIME_REPORT "Print -ftime-report for each TU" OFF)
set(_MAX_ERRORS 0 CACHE STRING "Stop each TU after N errors (0 = no limit, keeps -Werror)")

if(_TIME_REPORT)
    target_compile_options(${_dev} INTERFACE -ftime-report)
endif()

if(_MAX_ERRORS GREATER 0)
    target_compile_options(${_dev} INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-fmax-errors=${_MAX_ERRORS}>
        $<$<CXX_COMPILER_ID:GNU>:-fconcepts-diagnostics-depth=2>
        $<$<CXX_COMPILER_ID:Clang,AppleClang>:-ferror-limit=${_MAX_ERRORS}>
    )
else()
    target_compile_options(${_dev} INTERFACE -Werror)
endif()

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
    target_compile_options(${_dev} INTERFACE -O1)
endif()
if(_O2)
    target_compile_options(${_dev} INTERFACE -O2)
endif()
if(_O3)
    target_compile_options(${_dev} INTERFACE -O3)
endif()
if(_FAST_MATH)
    target_compile_options(${_dev} INTERFACE -ffast-math)
endif()

set(_san "")
if(_ENABLE_ASAN OR _ENABLE_LSAN)
    list(APPEND _san -fsanitize=address)
endif()
if(_ENABLE_TSAN)
    list(APPEND _san -fsanitize=thread)
endif()
if(_ENABLE_UBSAN)
    list(APPEND _san -fsanitize=undefined)
endif()
if(_san)
    target_compile_options(${_dev} INTERFACE ${_san} -fno-omit-frame-pointer -g)
    target_link_options(${_dev}    INTERFACE ${_san} -fno-omit-frame-pointer -g)
endif()
