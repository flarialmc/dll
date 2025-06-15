cmake_minimum_required(VERSION 3.24.0)
project(Flarial)

set(CMAKE_CXX_STANDARD 23)

# Enable parallel builds and faster configuration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Enable parallel processing for MSVC
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

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

# Cache git hash to avoid running git command every configure
if(NOT DEFINED COMMIT_HASH OR COMMIT_HASH STREQUAL "")
    execute_process(
            COMMAND git rev-parse --short HEAD
            OUTPUT_VARIABLE COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )
    set(COMMIT_HASH "${COMMIT_HASH}" CACHE STRING "Git commit hash")
endif()

add_compile_definitions(COMMIT_HASH="${COMMIT_HASH}")

# Common compile options
add_compile_options(/bigobj /utf-8)

# Build type specific optimizations
if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Test")
    # Optimize for performance - consolidated flags
    add_compile_options(/O2 /Oi /Ot /Ox /Oy /Gy /Gw /GF /fp:fast /Gd /MT /Ob2)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Optimize for build time
    add_compile_options(/Od /Zi /MT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# Cache build date to avoid regenerating every configure (optional for development speed)
if(NOT DEFINED FLARIAL_BUILD_DATE OR FLARIAL_BUILD_DATE STREQUAL "")
    # Use environment variable if available (set by CI), otherwise generate timestamp
    set(FLARIAL_BUILD_DATE "$ENV{BUILD_DATE}" CACHE STRING "Build date")
    if(FLARIAL_BUILD_DATE STREQUAL "")
        string(TIMESTAMP FLARIAL_BUILD_DATE "%Y %b %d %H:%M:%S")
        set(FLARIAL_BUILD_DATE "${FLARIAL_BUILD_DATE}" CACHE STRING "Build date")
    endif()
endif()

add_compile_definitions(
        FLARIAL_VERSION=\"${CLIENT_VERSION}\"
        FLARIAL_BUILD_TYPE=\"${CLIENT_BUILD_TYPE}\"
        FLARIAL_BUILD_DATE=\"${FLARIAL_BUILD_DATE}\"
)

# Use CONFIGURE_DEPENDS for better performance but still track changes
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
file(GLOB_RECURSE LIB_FILES CONFIGURE_DEPENDS "lib/*.cpp" "lib/*.h")
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
        EXCLUDE_FROM_ALL ON
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

# Enable persistent caching for FetchContent
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/_deps" CACHE STRING "Base directory for FetchContent")
set(FETCHCONTENT_QUIET ON)  # Reduce configure output noise

# Enable fully disconnected builds after first download - but only after cache exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/_deps/libhat-src")
    set(FETCHCONTENT_FULLY_DISCONNECTED ON CACHE BOOL "Disable all attempts to download or update content and assume source dirs already exist")
else()
    set(FETCHCONTENT_FULLY_DISCONNECTED OFF CACHE BOOL "Disable all attempts to download or update content and assume source dirs already exist")
endif()

# Optimize git for faster downloads
execute_process(
    COMMAND git config --global pack.windowMemory 512m
    COMMAND git config --global pack.packSizeLimit 2g
    COMMAND git config --global pack.threads 0
    COMMAND git config --global core.preloadindex true
    COMMAND git config --global core.fscache true
    ERROR_QUIET
)

# Create cache directory if it doesn't exist
file(MAKE_DIRECTORY "${FETCHCONTENT_BASE_DIR}")

# Enable parallel processing for dependencies
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N} CACHE STRING "Number of parallel build jobs")
    math(EXPR PARALLEL_DOWNLOADS "${N} / 2")
    if(PARALLEL_DOWNLOADS LESS 2)
        set(PARALLEL_DOWNLOADS 2)
    endif()
    set(FETCHCONTENT_PARALLEL ${PARALLEL_DOWNLOADS} CACHE STRING "Number of parallel FetchContent downloads")
endif()

# Check if dependencies are already populated to skip FetchContent entirely
set(SKIP_FETCHCONTENT FALSE)
if(EXISTS "${FETCHCONTENT_BASE_DIR}/entt-src" AND 
   EXISTS "${FETCHCONTENT_BASE_DIR}/nes-src" AND 
   EXISTS "${FETCHCONTENT_BASE_DIR}/libhat-src" AND 
   EXISTS "${FETCHCONTENT_BASE_DIR}/fmt-src" AND 
   EXISTS "${FETCHCONTENT_BASE_DIR}/magic_enum-src" AND 
   EXISTS "${FETCHCONTENT_BASE_DIR}/luabridge-src")
    set(SKIP_FETCHCONTENT TRUE)
    message(STATUS "All dependencies found in cache, skipping FetchContent")
endif()

if(NOT SKIP_FETCHCONTENT)
    # Optimized FetchContent declarations with git config optimizations
    FetchContent_Declare(
            entt
            GIT_REPOSITORY https://github.com/skypjack/entt.git
            GIT_TAG        f931687ff04d435871ac9664bb299f71f2a8fafc
            GIT_PROGRESS   ON
            GIT_SHALLOW    OFF
            GIT_SUBMODULES ""  # Skip submodules
            GIT_CONFIG     "advice.detachedHead=false;core.autocrlf=false;core.eol=lf"
    )

    FetchContent_Declare(
            nes
            GIT_REPOSITORY https://github.com/DisabledMallis/NuvolaEventSystem.git
            GIT_TAG        main
            GIT_SHALLOW    ON
            GIT_PROGRESS   ON
            GIT_SUBMODULES ""
            GIT_CONFIG     "advice.detachedHead=false;core.autocrlf=false;core.eol=lf"
    )

    FetchContent_Declare(
            libhat
            GIT_REPOSITORY https://github.com/BasedInc/libhat.git
            GIT_TAG        d6297514b05fd238f5b46d003325f108c22741e3
            GIT_SHALLOW    OFF
            GIT_PROGRESS   ON
            GIT_SUBMODULES ""
            GIT_CONFIG     "advice.detachedHead=false;core.autocrlf=false;core.eol=lf"
    )

    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG        master
            GIT_SHALLOW    ON
            GIT_PROGRESS   ON
            GIT_SUBMODULES ""
            GIT_CONFIG     "advice.detachedHead=false;core.autocrlf=false;core.eol=lf"
    )

    FetchContent_Declare(
            magic_enum
            GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
            GIT_TAG        master
            GIT_SHALLOW    ON
            GIT_PROGRESS   ON
            GIT_SUBMODULES ""
            GIT_CONFIG     "advice.detachedHead=false;core.autocrlf=false;core.eol=lf"
    )

    FetchContent_Declare(
            LuaBridge
            GIT_REPOSITORY https://github.com/kunitoki/LuaBridge3.git
            GIT_TAG        master
            GIT_SHALLOW    ON
            GIT_PROGRESS   ON
            GIT_SUBMODULES ""
            GIT_CONFIG     "advice.detachedHead=false;core.autocrlf=false;core.eol=lf"
    )

    # Use parallel processing for FetchContent
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
        # Enable parallel population for CMake 3.24+
        FetchContent_MakeAvailable(libhat entt nes fmt magic_enum LuaBridge)
    else()
        # Manual parallel downloads for older CMake versions
        FetchContent_Populate(libhat)
        FetchContent_Populate(entt)
        FetchContent_Populate(nes)
        FetchContent_Populate(fmt)
        FetchContent_Populate(magic_enum)
        FetchContent_Populate(LuaBridge)
        
        add_subdirectory(${libhat_SOURCE_DIR} ${libhat_BINARY_DIR})
        add_subdirectory(${entt_SOURCE_DIR} ${entt_BINARY_DIR})
        add_subdirectory(${nes_SOURCE_DIR} ${nes_BINARY_DIR})
        add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
        add_subdirectory(${magic_enum_SOURCE_DIR} ${magic_enum_BINARY_DIR})
        add_subdirectory(${luabridge_SOURCE_DIR} ${luabridge_BINARY_DIR})
    endif()
else()
    # Manually add the cached directories as subdirectories
    add_subdirectory("${FETCHCONTENT_BASE_DIR}/entt-src" "${FETCHCONTENT_BASE_DIR}/entt-build")
    add_subdirectory("${FETCHCONTENT_BASE_DIR}/nes-src" "${FETCHCONTENT_BASE_DIR}/nes-build")
    add_subdirectory("${FETCHCONTENT_BASE_DIR}/libhat-src" "${FETCHCONTENT_BASE_DIR}/libhat-build")
    add_subdirectory("${FETCHCONTENT_BASE_DIR}/fmt-src" "${FETCHCONTENT_BASE_DIR}/fmt-build")
    add_subdirectory("${FETCHCONTENT_BASE_DIR}/magic_enum-src" "${FETCHCONTENT_BASE_DIR}/magic_enum-build")
    add_subdirectory("${FETCHCONTENT_BASE_DIR}/luabridge-src" "${FETCHCONTENT_BASE_DIR}/luabridge-build")
endif()

target_link_libraries(Flarial PRIVATE libcurl ws2_32.lib crypt32.lib normaliz.lib wldap32.lib LuaBridge lua_static libhat fmt::fmt EnTT::EnTT NES windowscodecs.lib urlmon.lib dwrite.lib d3d12.lib dxgi.lib d3d11.lib d2d1.lib wininet.lib version FreeType magic_enum)
target_link_libraries(Flarial PUBLIC MinHook)

set_property(TARGET Flarial PROPERTY UNITY_BUILD ON)
set_property(TARGET Flarial PROPERTY UNITY_BUILD_BATCH_SIZE 32)

target_precompile_headers(Flarial PRIVATE "src/PCH.hpp")
