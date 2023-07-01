cmake_minimum_required(VERSION 3.24.0)
project(Flarial VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
add_compile_options($<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT> /GS)
endif()

file(GLOB_RECURSE sources "main.cpp" "src/**/*.cpp" "src/**/*.hpp" "lib/**/*.cpp")

add_library(Flarial SHARED ${sources} src/SDK/SDK.cpp src/Client/Events/Game/TickEvent.hpp src/Client/Hook/Hooks/Game/ActorNormalTick.cpp src/Client/Hook/Hooks/Game/ActorNormalTick.hpp)

target_include_directories(Flarial PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/include/")
add_library(sussy SHARED IMPORTED GLOBAL)
set_target_properties(sussy PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook/minhook.lib")


target_link_libraries(Flarial PRIVATE windowscodecs.lib urlmon.lib dwrite.lib d3d12.lib dxgi.lib d3d11.lib d2d1.lib version)
target_link_libraries(Flarial PUBLIC sussy)