#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Utils/WinrtUtils.hpp>

class FSLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<FSLib>("fs")
                .addStaticFunction("exists", [](const std::string& path) -> bool {
                    return std::filesystem::exists(std::filesystem::path(Utils::getClientPath() + "\\" + path).lexically_normal());
                })
                .addStaticFunction("isDirectory", [](const std::string& path) -> bool {
                    return std::filesystem::is_directory(std::filesystem::path(Utils::getClientPath() + "\\" + path).lexically_normal());
                })
                .addStaticFunction("create", [](const std::string& path) -> bool {
                    return std::filesystem::create_directory(std::filesystem::path(Utils::getClientPath() + "\\" + path).lexically_normal());
                })
                .addStaticFunction("remove", [](const std::string& path) -> bool {
                    return std::filesystem::remove(std::filesystem::path(Utils::getClientPath() + "\\" + path).lexically_normal());
                })
                .addStaticFunction("readFile", [](const std::string& path) -> std::string {
                    std::ifstream file(std::filesystem::path(Utils::getClientPath() + "\\" + path).lexically_normal(), std::ios::in);

                    if (!file.is_open()) {
                        throw std::runtime_error("Failed to open file for reading: " + path);
                    }

                    std::string content((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
                    return content;
                })
                .addStaticFunction("writeFile", [](const std::string& path, const std::string& content) -> bool {
                    std::ofstream file(std::filesystem::path(Utils::getClientPath() + "\\" + path).lexically_normal(), std::ios::app);

                    if (!file.is_open()) {
                        return false;
                    }

                    file << content;
                    return true;
                })
                .addStaticFunction("listDirectory", [](const std::string& path) -> std::vector<std::string> {
                    std::vector<std::string> files;
                    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::path(Utils::getClientPath() + "\\" + path).lexically_normal())) {
                        files.push_back(entry.path().string());
                    }
                    return files;
                })
                /*
                .addStaticFunction("open", [](const std::optional<std::string>& path) {
                    std::string dir = "Flarial" + (path ? ("\\" + *path) : "");
                    WinrtUtils::openSubFolder(std::filesystem::path(dir).lexically_normal().string());
                })
                */
            .endClass();
    }
};
