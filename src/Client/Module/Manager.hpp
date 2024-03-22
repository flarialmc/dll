#pragma once

#include <vector>
#include "Modules/Module.hpp"
// TODO make moduleMap AND modules but use moduleMap for search
namespace ModuleManager {
    extern std::unordered_map<size_t, Module*> moduleMap;

    void initialize();

    void terminate();

    void addModule(Module *module);

    std::vector<Module *> getModules();

    void SaveModulesConfig();

    bool doesAnyModuleHave(const std::string& settingName);

    Module *getModule(const std::string& name);

    extern std::vector<std::string> onlineUsers;
}
