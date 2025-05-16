#=== LIBRARY: cimgui + Dear ImGui
add_library(cimgui STATIC
        cimgui/cimgui.cpp
        cimgui/cimgui.h
        cimgui/imgui.cpp
        cimgui/imgui.h
        cimgui/imgui_widgets.cpp
        cimgui/imgui_draw.cpp
        cimgui/imgui_tables.cpp
        cimgui/imgui_demo.cpp)
target_include_directories(cimgui INTERFACE cimgui)

#=== LIBRARY: sokol
# add headers to the the file list because they are useful to have in IDEs
set(SOKOL_HEADERS
        sokol/sokol_gfx.h
        sokol/sokol_app.h
        sokol/sokol_imgui.h
        sokol/sokol_glue.h)
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
    target_compile_options(sokol PRIVATE -x objective-c)
    target_link_libraries(sokol PUBLIC
            "-framework QuartzCore"
            "-framework Cocoa"
            "-framework MetalKit"
            "-framework Metal")
else()
    add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
    if (CMAKE_SYSTEM_NAME STREQUAL Linux)
        target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL dl m)
        target_link_libraries(sokol PUBLIC Threads::Threads)
    endif()
endif()
target_link_libraries(sokol PUBLIC cimgui)
target_include_directories(sokol INTERFACE sokol)

#=== Setup single EXTERN
add_library(extern INTERFACE)
target_link_libraries(extern INTERFACE sokol)

# Header-Only libraries
target_include_directories(extern INTERFACE cglm)