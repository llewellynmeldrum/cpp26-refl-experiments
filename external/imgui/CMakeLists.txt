cmake_minimum_required(VERSION 3.28)

add_library(imgui STATIC
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp

    backends/imgui_impl_sdl3.cpp
    backends/imgui_impl_sdlrenderer3.cpp
    misc/cpp/imgui_stdlib.cpp

    imgui_demo.cpp
)

find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(imgui PUBLIC
    SDL3::SDL3
)


target_include_directories(imgui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp"
)
# Do we need to add openGL here?
target_compile_features(imgui PUBLIC cxx_std_23)

# disable imgui warnings
target_compile_options(imgui PRIVATE
    -w
)
