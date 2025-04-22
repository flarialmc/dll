#pragma once

#include "ScriptLib.hpp"

#include <Client/GUI/Engine/Engine.hpp>
#include <Client/Command/CommandManager.hpp>

class ClientLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("client")
                .addFunction("notify", [](const std::string& message) {
                    if (message.empty()) return;
                    FlarialGUI::Notify(message);
                })
                .addFunction("yeet", []() {
                    std::exit(0);
                })
                .addFunction("freeMouse", []() {
                    if (!SDK::clientInstance) return;
                    SDK::clientInstance->releaseMouse();
                })
                .addFunction("grabMouse", []() {
                    if (!SDK::clientInstance) return;
                    SDK::clientInstance->grabMouse();
                })
                .addFunction("getScreenName", []() -> std::string {
                    if (!SDK::clientInstance) return "";
                    return SDK::clientInstance->getScreenName();
                })
/*
                .addFunction("commands", [](lua_State* L) {

                    lua_newtable(L);
                    int tableIndex = lua_gettop(L);

                    int i = 1;
                    for (const auto& cmd : CommandManager::Commands) {
                        lua_newtable(L);

                        lua_pushstring(L, "name");
                        lua_pushstring(L, cmd->Name.c_str());
                        lua_settable(L, -3);

                        lua_pushstring(L, "description");
                        lua_pushstring(L, cmd->Description.c_str());
                        lua_settable(L, -3);

                        lua_pushstring(L, "aliases");
                        lua_newtable(L);

                        for (size_t j = 0; j < cmd->Aliases.size(); ++j) {
                            lua_pushinteger(L, j + 1);
                            lua_pushstring(L, cmd->Aliases[j].c_str());
                            lua_settable(L, -3);
                        }

                        lua_settable(L, -3);
                        lua_rawseti(L, tableIndex, i++);
                    }
                    return 1;
                })
*/
            .endNamespace();
    }
};