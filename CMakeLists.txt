cmake_minimum_required(VERSION 3.24.0)
project(Flarial VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)

execute_process(
        COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/commit_hash.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/commit_hash.h
        @ONLY
)

add_definitions(-DCOMMIT_HASH="${COMMIT_HASH}")

if(MSVC)
    add_compile_options(/MT)

    # Optimize for speed
    add_compile_options($<$<CONFIG:Release>:/O2>)
    add_compile_options($<$<CONFIG:Release>:/Ot>)
    add_compile_options($<$<CONFIG:Release>:/Ox>)
    add_compile_options($<$<CONFIG:Release>:/Oy>)

    # Enable intrinsic functions
    add_compile_options($<$<CONFIG:Release>:/Oi>)

    # Enable link-time code generation for whole program optimization
    add_compile_options($<$<CONFIG:Release>:/GL>)

    # Separate functions for the linker to improve the optimization process
    add_compile_options($<$<CONFIG:Release>:/Gy>)

    # Optimize global data to reduce the binary size
    add_compile_options($<$<CONFIG:Release>:/Gw>)

    # Enable string pooling to reduce binary size by consolidating duplicate strings
    add_compile_options($<$<CONFIG:Release>:/GF>)

    # Optimize floating point operations for speed (may introduce minor precision differences)
    # add_compile_options($<$<CONFIG:Release>:/fp:fast>)

    # Disable RTTI (Run-Time Type Information) to reduce binary size and possibly improve performance
    add_compile_options($<$<CONFIG:Release>:/GR->)

    # Inline any suitable function to improve performance by reducing function call overhead
    add_compile_options($<$<CONFIG:Release>:/Ob2>)

#    add_compile_options("$<$<CONFIG:RELEASE>:/Zi>")
#    add_link_options("$<$<CONFIG:RELEASE>:/DEBUG>") # Enable debug symbols in release for edge cases.
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


file(GLOB_RECURSE sources "src/**/*.cpp" "src/**/*.hpp" "lib/**/*.cpp" "src/Client/GUI/Engine/*.cpp")

add_library(${PROJECT_NAME} SHARED ${sources} main.cpp src/Assets/Assets.rc)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/include/" "${CMAKE_CURRENT_LIST_DIR}/lib/glm/" "${CMAKE_CURRENT_LIST_DIR}/lib/ImGui")

# Disable RTTI
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-rtti)
elseif (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /GR-)
endif()

add_library(MinHook SHARED IMPORTED GLOBAL)
set_target_properties(MinHook PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook/minhook.lib")

FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
)

FetchContent_Declare(
        nes
        GIT_REPOSITORY https://github.com/DisabledMallis/NuvolaEventSystem.git
        GIT_TAG main
)

FetchContent_Declare(
        libhat
        GIT_REPOSITORY  https://github.com/BasedInc/libhat.git
        GIT_TAG         9ef05d6961ce37a4c801f11159de895aa21878a9
)
set(ENTT_BUILD_TESTING OFF)  # Disable building tests
FetchContent_MakeAvailable(libhat entt nes)

target_link_libraries(${PROJECT_NAME} PRIVATE libhat EnTT::EnTT NES windowscodecs.lib urlmon.lib dwrite.lib d3d12.lib dxgi.lib d3d11.lib d2d1.lib wininet.lib version)
target_link_libraries(${PROJECT_NAME} PUBLIC MinHook)

target_precompile_headers(Flarial PRIVATE "src/PCH.cpp")