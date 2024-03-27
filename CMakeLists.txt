cmake_minimum_required(VERSION 3.24.0)
project(Flarial VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    add_compile_options(/MT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()



# Default to statically-linked runtime.
if("${MSVC_RUNTIME}" STREQUAL "")
    set(MSVC_RUNTIME "static")

    if(${MSVC_RUNTIME} STREQUAL "static")
        message(STATUS
                "MSVC -> forcing use of statically-linked runtime."
        )
        foreach(variable ${variables})
            if(${variable} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
            endif()
        endforeach()
    else()
        message(STATUS
                "MSVC -> forcing use of dynamically-linked runtime."
        )
        foreach(variable ${variables})
            if(${variable} MATCHES "/MT")
                string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
            endif()
        endforeach()
    endif()
endif()


file(GLOB_RECURSE sources "main.cpp" "src/**/*.cpp" "src/**/*.hpp" "lib/**/*.cpp" "src/Client/GUI/Engine/*.cpp")

add_library(${PROJECT_NAME} SHARED ${sources} src/SDK/Client/Render/Matrix.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/include/" "${CMAKE_CURRENT_LIST_DIR}/lib/glm/")

# Disable RTTI
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-rtti)
elseif (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /GR-)
endif()

add_library(MinHook SHARED IMPORTED GLOBAL)
set_target_properties(MinHook PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook/minhook.lib")

FetchContent_Declare(
        libhat
        GIT_REPOSITORY  https://github.com/BasedInc/libhat.git
        GIT_TAG         9ef05d6961ce37a4c801f11159de895aa21878a9
)
FetchContent_MakeAvailable(libhat)
target_link_libraries(${PROJECT_NAME} PRIVATE libhat windowscodecs.lib urlmon.lib dwrite.lib d3d12.lib dxgi.lib d3d11.lib d2d1.lib wininet.lib version)
target_link_libraries(${PROJECT_NAME} PUBLIC MinHook)