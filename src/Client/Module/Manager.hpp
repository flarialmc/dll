#pragma once

#include <vector>
#include "Modules/Module.hpp"
#include "../Events/Listener.hpp"

// TODO make moduleMap AND modules but use moduleMap for search
namespace ModuleManager {
    extern std::unordered_map<size_t, std::shared_ptr<Module>> moduleMap;
    extern std::vector<std::shared_ptr<Listener>> services;

    extern bool initialized;
    extern bool restartModules;

    void initialize();

    void terminate();

    template<typename T, typename... ArgsT>
    void addModule(ArgsT... args) {
        auto modulePtr = std::make_shared<T>(args...);
        size_t hash = std::hash<std::string>{}(modulePtr->name);
        moduleMap[hash] = modulePtr;
    }

    template<typename T, typename... ArgsT>
    void addService(ArgsT... args) {
        auto servicePtr = std::make_shared<T>(args...);
        services.emplace_back(servicePtr);
    }

    std::vector<std::shared_ptr<Module>> getModules();

    void syncState();

    void SaveModulesConfig();

    bool doesAnyModuleHave(const std::string& settingName);

    std::shared_ptr<Module> getModule(const std::string& name);
}

