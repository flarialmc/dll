#pragma once

#include "ScriptLib.hpp"

#include <curl/curl/curl.h>
#include <thread>

class NetworkLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("network")
                .addFunction("get", [](const std::string& url) -> std::string {
                    CURL* curl = curl_easy_init();
                    std::string response;

                    if (curl) {
                        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                        curl_easy_perform(curl);
                        curl_easy_cleanup(curl);
                    }
                    return response;
                })
                .addFunction("post", [](const std::string& url, const std::string& data) -> std::string {
                    CURL* curl = curl_easy_init();
                    std::string response;

                    if (curl) {
                        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                        curl_easy_setopt(curl, CURLOPT_POST, 1L);
                        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                        curl_easy_perform(curl);
                        curl_easy_cleanup(curl);
                    }
                    return response;
                })
                .addFunction("getAsync", [](const std::string& url, luabridge::LuaRef callback, lua_State* L) {
                    if (!callback.isFunction()) {
                        luaL_error(L, "network.getAsync: second argument must be a callback function");
                        return;
                    }

                    auto* script = getScriptFromState(L);
                    if (!script) return;

                    // Store callback in Lua registry so it survives across frames
                    callback.push(L);
                    int callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
                    auto weakScript = script->weak_from_this();

                    std::thread([url, callbackRef, weakScript]() {
                        CURL* curl = curl_easy_init();
                        std::string response;
                        long statusCode = 0;
                        bool success = false;

                        if (curl) {
                            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
                            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
                            success = (curl_easy_perform(curl) == CURLE_OK);
                            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
                            curl_easy_cleanup(curl);
                        }

                        if (auto script = weakScript.lock()) {
                            script->queueCallback([callbackRef, response = std::move(response), statusCode, success](lua_State* L) {
                                lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
                                if (lua_isfunction(L, -1)) {
                                    lua_pushstring(L, response.c_str());
                                    lua_pushinteger(L, statusCode);
                                    lua_pushboolean(L, success);
                                    if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
                                        std::string err = lua_tostring(L, -1);
                                        Logger::script(true, "network.getAsync callback error: {}", err);
                                        lua_pop(L, 1);
                                    }
                                } else {
                                    lua_pop(L, 1);
                                }
                                luaL_unref(L, LUA_REGISTRYINDEX, callbackRef);
                            });
                        }
                    }).detach();
                })
                .addFunction("postAsync", [](const std::string& url, const std::string& data, luabridge::LuaRef callback, lua_State* L) {
                    if (!callback.isFunction()) {
                        luaL_error(L, "network.postAsync: third argument must be a callback function");
                        return;
                    }

                    auto* script = getScriptFromState(L);
                    if (!script) return;

                    callback.push(L);
                    int callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
                    auto weakScript = script->weak_from_this();

                    std::thread([url, data, callbackRef, weakScript]() {
                        CURL* curl = curl_easy_init();
                        std::string response;
                        long statusCode = 0;
                        bool success = false;

                        if (curl) {
                            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                            curl_easy_setopt(curl, CURLOPT_POST, 1L);
                            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
                            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
                            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
                            success = (curl_easy_perform(curl) == CURLE_OK);
                            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
                            curl_easy_cleanup(curl);
                        }

                        if (auto script = weakScript.lock()) {
                            script->queueCallback([callbackRef, response = std::move(response), statusCode, success](lua_State* L) {
                                lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
                                if (lua_isfunction(L, -1)) {
                                    lua_pushstring(L, response.c_str());
                                    lua_pushinteger(L, statusCode);
                                    lua_pushboolean(L, success);
                                    if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
                                        std::string err = lua_tostring(L, -1);
                                        Logger::script(true, "network.postAsync callback error: {}", err);
                                        lua_pop(L, 1);
                                    }
                                } else {
                                    lua_pop(L, 1);
                                }
                                luaL_unref(L, LUA_REGISTRYINDEX, callbackRef);
                            });
                        }
                    }).detach();
                })
            .endNamespace();
    }

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append(static_cast<char *>(contents), totalSize);
        return totalSize;
    }

    static Script* getScriptFromState(lua_State* L) {
        lua_getglobal(L, "_script_instance");
        if (!lua_islightuserdata(L, -1)) {
            lua_pop(L, 1);
            luaL_error(L, "Script instance not found");
            return nullptr;
        }
        auto* script = static_cast<Script*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        return script;
    }
};
