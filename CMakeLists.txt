cmake_minimum_required(VERSION 3.24.0)
project(Flarial)

set(CMAKE_CXX_STANDARD 23)

set(CLIENT_VERSION "2.00") # Update this for every new release

set(CLIENT_BUILD_TYPE "Release")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CLIENT_BUILD_TYPE "Debug")
    add_compile_definitions(__DEBUG__)
elseif (CMAKE_BUILD_TYPE STREQUAL "Test")
    set(CLIENT_BUILD_TYPE "Test")
    add_compile_definitions(__TEST__)
else()
    set(CLIENT_BUILD_TYPE "Release")
    add_compile_definitions(__RELEASE__)
endif()

execute_process(
        COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_compile_definitions(COMMIT_HASH="${COMMIT_HASH}")

add_compile_options(/bigobj)
add_compile_options(/utf-8)
if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Test")
    # Optimize for performance
    add_compile_options(/O2 /Oi /Gy /Gw /GF /fp:fast /GR- /Gd /MT /Ob2)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Optimize for build time
    add_compile_options(/Od /Zi /MT) #/MTd causes static asserts to fail
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# Generate version and build date
string(TIMESTAMP BUILD_DATE "%Y %b %d %H:%M:%S")
set(FLARIAL_BUILD_DATE "${BUILD_DATE}")

add_compile_definitions(
        FLARIAL_VERSION=\"${CLIENT_VERSION}\"
        FLARIAL_BUILD_TYPE=\"${CLIENT_BUILD_TYPE}\"
        FLARIAL_BUILD_DATE=\"${FLARIAL_BUILD_DATE}\"
)


file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE LIB_FILES "lib/*.cpp")
add_library(Flarial SHARED main.cpp ${SOURCE_FILES} ${LIB_FILES} src/Assets/Assets.rc)

target_include_directories(Flarial PRIVATE
        "."
        "src"
        "src/Client"
        "src/Client/Module"
        "lib"
        "lib/lua/lua-5.4.7/include"
)

add_subdirectory(lib/lua)
add_subdirectory(lib/freetype)

add_library(MinHook SHARED IMPORTED GLOBAL)
set_target_properties(MinHook PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook/minhook.lib")

add_library(FreeType SHARED IMPORTED GLOBAL)
set_target_properties(FreeType PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/freetype/libs/freetype.lib")

target_link_libraries(Flarial PRIVATE lua_static libhat fmt::fmt EnTT::EnTT NES windowscodecs.lib urlmon.lib dwrite.lib d3d12.lib dxgi.lib d3d11.lib d2d1.lib wininet.lib version FreeType magic_enum)
target_link_libraries(Flarial PUBLIC MinHook)

target_precompile_headers(Flarial PRIVATE "src/PCH.cpp")

include(FetchContent)

FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG        f931687ff04d435871ac9664bb299f71f2a8fafc
)

FetchContent_Declare(
        nes
        GIT_REPOSITORY https://github.com/DisabledMallis/NuvolaEventSystem.git
        GIT_TAG        main
)

FetchContent_Declare(
        libhat
        GIT_REPOSITORY https://github.com/BasedInc/libhat.git
        GIT_TAG        d6297514b05fd238f5b46d003325f108c22741e3
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        master
)

FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(libhat entt nes fmt magic_enum)