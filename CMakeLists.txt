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

file(GLOB sources
		"main.cpp",
		"src/**/*.cpp",
		"src/**/*.hpp"
		)

add_library(Flarial SHARED ${sources} src/Client/Managers/Module/Module.hpp src/Utils/Logger/Logger.cpp)

# target_include_directories(Floral PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Lib/include/")
target_link_libraries(Flarial PRIVATE version)