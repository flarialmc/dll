#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>

class FSLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<FSLib>("fs")
                .addStaticFunction("exists", [](const std::string& path) -> bool {
                    return std::filesystem::exists(path);
                })
                .addStaticFunction("isDirectory", [](const std::string& path) -> bool {
                    return std::filesystem::is_directory(path);
                })
                .addStaticFunction("create", [](const std::string& path) -> bool {
                    return std::filesystem::create_directory(path);
                })
                .addStaticFunction("remove", [](const std::string& path) -> bool {
                    return std::filesystem::remove(path);
                })
                .addStaticFunction("readFile", [](const std::string& path) -> std::string {
                    std::ifstream file(path, std::ios::in);
                    if (!file.is_open()) {
                        throw std::runtime_error("Failed to open file for reading: " + path);
                    }
                    std::string content((std::istreambuf_iterator<char>(file)),
                                         std::istreambuf_iterator<char>());
                    return content;
                })
                .addStaticFunction("writeFile", [](const std::string& path, const std::string& content) -> bool {
                    std::ofstream file(path, std::ios::out);
                    if (!file.is_open()) {
                        return false;
                    }
                    file << content;
                    return true;
                })
                .addStaticFunction("listDirectory", [](const std::string& path) -> std::vector<std::string> {
                    std::vector<std::string> files;
                    for (const auto& entry : std::filesystem::directory_iterator(path)) {
                        files.push_back(entry.path().string());
                    }
                    return files;
                })
            .endClass();
    }
};