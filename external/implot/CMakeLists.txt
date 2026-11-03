cmake_minimum_required(VERSION 3.28)

add_library(implot STATIC
    implot.cpp
    implot_items.cpp
    implot_demo.cpp
)

# target_compile_definitions(imgui PUBLIC
#     IMGUI_USER_CONFIG="../../include/my_imconfig.hpp"
# )

# GLM dep
# target_include_directories(imgui PRIVATE
#     "${CMAKE_CURRENT_SOURCE_DIR}../OpenGL-Mathematics"
# )

# target_compile_definitions(imgui
#     PUBLIC GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#     PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE
# )
# SDL3 dep
# find_package(SDL3 REQUIRED CONFIG)
# target_link_libraries(imgui PUBLIC
#     SDL3::SDL3
# )
# libassert dep
# find_package(SDL3 REQUIRED CONFIG)
target_include_directories(implot PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(implot PUBLIC imgui)
#target_link_libraries(imgui PRIVATE
#    libassert::assert
#)


target_compile_features(implot PUBLIC cxx_std_23)

target_compile_options(implot PRIVATE -w)
