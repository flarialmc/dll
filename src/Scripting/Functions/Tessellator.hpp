#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/GUI/Engine/Constraints.hpp"
#include "Scripting/LUAHelper.hpp"
#include <string>
#include <codecvt>
#include <cmath>

namespace LuaTessellator {
    void registerConstraints(lua_State *L) {
        LUAHelper(L)
                .getClass("Tessellator")
                .registerLambdaFunction("getCurrentScreen", [](lua_State *L) -> int {
                    return 0;
                })
                .registerLambdaFunction("getCurrentScreen", [](lua_State *L) -> int {
                    return 0;
                });
    }
}
