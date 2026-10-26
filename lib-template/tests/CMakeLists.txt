# Every tests/test_*.cpp becomes its own executable and its own CTest test.
# Prefixing with the project name avoids clashing with a consumer's targets
# if they ever build our tests via add_subdirectory.
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

foreach(src IN LISTS TEST_SOURCES)
    get_filename_component(name "${src}" NAME_WE)
    set(target ${PROJECT_NAME}_${name})
    add_executable(${target} "${src}")
    set_target_properties(${target} PROPERTIES OUTPUT_NAME ${name})
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME} ${PROJECT_NAME}_dev_flags)
    add_test(NAME ${name} COMMAND ${target})
endforeach()

# -----------------------------------------------------------------------------
# Header self-containment check.
# For every public header, generate a .cpp that includes it twice and nothing else.
# Fails the build if a header relies on something it does not include itself
# (the "copy six headers to get one" problem), or lacks an include guard.
# -----------------------------------------------------------------------------
set(_tus "")
foreach(h IN LISTS LIB_HEADERS)
    file(RELATIVE_PATH rel "${PROJECT_SOURCE_DIR}/include" "${h}")
    string(MAKE_C_IDENTIFIER "${rel}" id)
    set(tu "${CMAKE_CURRENT_BINARY_DIR}/header_check/${id}.cpp")
    file(CONFIGURE OUTPUT "${tu}" CONTENT "#include <${rel}>\n#include <${rel}>\n")
    list(APPEND _tus "${tu}")
endforeach()

if(_tus)
    add_library(${PROJECT_NAME}_header_check OBJECT ${_tus})
    target_link_libraries(${PROJECT_NAME}_header_check PRIVATE ${PROJECT_NAME}::${PROJECT_NAME} ${PROJECT_NAME}_dev_flags)
endif()
