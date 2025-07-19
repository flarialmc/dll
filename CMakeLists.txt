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

# Optimize for speed
add_compile_options($<$<CONFIG:Release>:/O2>)
add_compile_options($<$<CONFIG:Release>:/Ot>)
add_compile_options($<$<CONFIG:Release>:/Ox>)
add_compile_options($<$<CONFIG:Release>:/Oy>)

# Enable intrinsic functions
add_compile_options($<$<CONFIG:Release>:/Oi>)

# Separate functions for the linker to improve the optimization process
add_compile_options($<$<CONFIG:Release>:/Gy>)

# Optimize global data to reduce the binary size
add_compile_options($<$<CONFIG:Release>:/Gw>)

# Enable string pooling to reduce binary size by consolidating duplicate strings
add_compile_options($<$<CONFIG:Release>:/GF>)

# Optimize floating point operations for speed (may introduce minor precision differences)
# add_compile_options($<$<CONFIG:Release>:/fp:fast>)

# Inline any suitable function to improve performance by reducing function call overhead
add_compile_options($<$<CONFIG:Release>:/Ob2>)

add_compile_options(/bigobj)
add_compile_options(/utf-8)
if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Test")
    # Optimize for performance
    add_compile_options(/O2 /Oi /Gy /Gw /GF /fp:fast /Gd /MT /Ob2)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Optimize for build time
    add_compile_options(/Od /Zi /MT)
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

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp")
file(GLOB_RECURSE LIB_FILES "lib/*.cpp" "lib/*.h")
add_library(Flarial SHARED main.cpp ${SOURCE_FILES} ${LIB_FILES} src/Assets/Assets.rc)

target_include_directories(Flarial PRIVATE
        "."
        "src"
        "src/Client"
        "src/Client/Module"
        "lib"
        "lib/lua/lua-5.4.7/include"
)

add_library(miniz STATIC lib/miniz/miniz.c)
target_include_directories(miniz PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/miniz")

target_link_libraries(Flarial PRIVATE miniz)

set_target_properties(miniz PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
)

add_subdirectory(lib/lua)
add_subdirectory(lib/freetype)


add_library(MinHook SHARED IMPORTED GLOBAL)
set_target_properties(MinHook PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook/minhook.lib")

add_library(FreeType SHARED IMPORTED GLOBAL)
set_target_properties(FreeType PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/freetype/libs/freetype.lib")

add_library(libcurl SHARED IMPORTED GLOBAL)
set_target_properties(libcurl PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
)
set_target_properties(libcurl PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/libcurl.lib")

add_library(zlib SHARED IMPORTED GLOBAL)
set_target_properties(zlib PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
)
set_target_properties(zlib PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/zlib.lib")

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

FetchContent_Declare(
        LuaBridge
        GIT_REPOSITORY https://github.com/kunitoki/LuaBridge3.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(libhat entt nes fmt magic_enum LuaBridge)

target_link_libraries(Flarial PRIVATE libcurl ws2_32.lib crypt32.lib normaliz.lib wldap32.lib LuaBridge lua_static libhat fmt::fmt EnTT::EnTT NES windowscodecs.lib urlmon.lib dwrite.lib d3d12.lib dxgi.lib d3d11.lib d2d1.lib wininet.lib setupapi.lib version FreeType magic_enum runtimeobject)

target_link_libraries(Flarial PUBLIC MinHook)

set_property(TARGET Flarial PROPERTY UNITY_BUILD ON)
set_property(TARGET Flarial PROPERTY UNITY_BUILD_BATCH_SIZE 32)

target_precompile_headers(Flarial PRIVATE "src/PCH.hpp")
