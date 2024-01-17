#pragma once

#include <vector>
#include "Modules/Module.hpp"

namespace ModuleManager
{
    extern std::vector<Module*> modules;
    void initialize();
    void terminate();
    void SaveModulesConfig();
    bool doesAnyModuleHave(std::string settingName);
    Module* getModule(std::string name);
    extern std::vector<std::string> OnlineUsers;
};
