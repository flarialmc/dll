#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>

int main()
{
    lua_State *l = luaL_newstate();
    double lversion = lua_version(l);
    printf("Lua version is %f\n", lversion);
}