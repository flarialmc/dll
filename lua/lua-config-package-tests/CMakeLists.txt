cmake_minimum_required(VERSION 3.1)
project(lua-config-package-test C)

find_package(Lua REQUIRED CONFIG)

add_executable(lua-config-package-build-test-using-static-target "./lua-config-package-build-test.c")
target_link_libraries(lua-config-package-build-test-using-static-target Lua::lua_static)

add_executable(lua-config-package-build-test-using-shared-target "./lua-config-package-build-test.c")
target_link_libraries(lua-config-package-build-test-using-shared-target Lua::lua_shared)

add_executable(lua-config-package-build-test-using-variables "./lua-config-package-build-test.c")
target_link_libraries(lua-config-package-build-test-using-variables "${LUA_LIBRARIES}")

add_library(lua-lib-module-compatible-mode UNKNOWN IMPORTED "${LUA_LIBRARY}")
set_target_properties(lua-lib-module-compatible-mode PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
        IMPORTED_LOCATION "${LUA_LIBRARY}")
target_link_libraries(lua-lib-module-compatible-mode INTERFACE "${LUA_LIBRARIES}")

add_executable(lua-config-package-build-test-using-module-compatible-mode "./lua-config-package-build-test.c")
target_link_libraries(lua-config-package-build-test-using-module-compatible-mode lua-lib-module-compatible-mode)
