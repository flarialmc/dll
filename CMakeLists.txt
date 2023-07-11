cmake_minimum_required(VERSION 3.24.0)
project(Flarial VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    add_compile_options(/MD /MP)
endif()


file(GLOB_RECURSE sources "main.cpp" "src/**/*.cpp" "src/**/*.hpp" "lib/**/*.cpp")

add_library(Flarial SHARED ${sources})

target_include_directories(Flarial PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/include/")

# Disable RTTI
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Flarial PRIVATE -fno-rtti)
elseif (MSVC)
    target_compile_options(Flarial PRIVATE /GR-)
endif()

add_library(sussy SHARED IMPORTED GLOBAL)
set_target_properties(sussy PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook/minhook.lib")

target_link_libraries(Flarial PRIVATE windowscodecs.lib urlmon.lib dwrite.lib d3d12.lib dxgi.lib d3d11.lib d2d1.lib wininet.lib version)
target_link_libraries(Flarial PUBLIC sussy)