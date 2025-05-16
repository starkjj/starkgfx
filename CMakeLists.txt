cmake_minimum_required(VERSION 3.20)
project(starkgfx)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Linux -pthread shenanigans
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

#=== LIBRARIES
add_subdirectory(libs)

#=== EXECUTABLE: starkgfx
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(starkgfx WIN32 src/game.c)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT starkgfx)
else()
    add_executable(starkgfx src/game.c)
endif()

#=== LINKING
target_link_libraries(starkgfx mingw32 extern)

# Emscripten-specific linker options
if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    # use our own minimal shell.html
    target_link_options(starkgfx PRIVATE --shell-file ../sokol/shell.html)
    # link with WebGL2
    target_link_options(starkgfx PRIVATE -sUSE_WEBGL2=1)
    # WASM+JS size optimizations
    target_link_options(starkgfx PRIVATE -sNO_FILESYSTEM=1 -sASSERTIONS=0 -sMALLOC=emmalloc --closure=1)
endif()

# explicitly strip dead code
if (CMAKE_C_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    target_link_options(starkgfx PRIVATE LINKER:-dead_strip)
endif()

# this hack removes the xxx-CMakeForceLinker.cxx dummy file
#set_target_properties(starkgfx PROPERTIES LINKER_LANGUAGE C)
