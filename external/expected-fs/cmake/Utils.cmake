#
# Print a message only if the `VERBOSE_OUTPUT` option is on
#

function(verbose_message content)
    if(${CMAKE_PROJECT_NAME}_VERBOSE_OUTPUT)
			message(STATUS ${content})
    endif()
endfunction()

#
# Add a target for formating the project using `clang-format` (i.e: cmake --build build --target clang-format)
#

function(add_clang_format_target)
    if(NOT ${PROJECT_NAME}_CLANG_FORMAT_BINARY)
        find_program(${PROJECT_NAME}_CLANG_FORMAT_BINARY clang-format)
    endif()

    if(${PROJECT_NAME}_CLANG_FORMAT_BINARY)
        set(format_sources ${headers})
        if(${PROJECT_NAME}_BUILD_EXECUTABLE)
            list(APPEND format_sources ${exe_sources})
        elseif(NOT ${PROJECT_NAME}_BUILD_HEADERS_ONLY)
            list(APPEND format_sources ${sources})
        endif()

        foreach(test_source IN LISTS test_sources)
            if(IS_ABSOLUTE "${test_source}")
                list(APPEND format_sources "${test_source}")
            else()
                list(APPEND format_sources "test/${test_source}")
            endif()
        endforeach()

        add_custom_target(clang-format
            COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY}
            -i ${format_sources}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

        message(STATUS "Format the project using the `clang-format` target (i.e: cmake --build build --target clang-format).\n")
    endif()
endfunction()
