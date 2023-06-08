cmake_minimum_required(VERSION 3.23.0)
project(Flarial VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
add_compile_options(
		$<$<CONFIG:>:/MT> 
		$<$<CONFIG:Debug>:/MTd> 
		$<$<CONFIG:Release>:/MT> 
)
endif()

file(GLOB_RECURSE sources
	"main.cpp",
	"src/**/*.cpp",
	"src/**/*.hpp"
	"lib/**/*.cpp",
)

add_library(Flarial SHARED ${sources})
target_include_directories(Flarial PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/include/")
target_link_libraries(Flarial PRIVATE dwrite.lib d3d12.lib dxgi.lib d3d11.lib d2d1.lib version)