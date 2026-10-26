# Every examples/*.cpp becomes an executable. `make run EX=<name>` runs one.
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(src IN LISTS EXAMPLE_SOURCES)
    get_filename_component(name "${src}" NAME_WE)
    set(target ${PROJECT_NAME}_example_${name})
    add_executable(${target} "${src}")
    set_target_properties(${target} PROPERTIES OUTPUT_NAME ${name})
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME} ${PROJECT_NAME}_dev_flags)
endforeach()
