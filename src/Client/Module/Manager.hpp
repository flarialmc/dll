#pragma once

#include <vector>
#include "Modules/Module.hpp"
#include "../Events/Listener.hpp"

// TODO make moduleMap AND modules but use moduleMap for search
namespace ModuleManager {
    extern std::map<size_t, std::shared_ptr<Module>> moduleMap;
    extern std::vector<std::shared_ptr<Listener>> services;

    extern bool initialized;
    extern bool restartModules;
    extern bool cguiRefresh;

    void initialize();
    void registerAllModules();
    void restart();
    void terminate();

    template<typename T, typename... ArgsT>
    void addModule(ArgsT... args) {
        auto modulePtr = std::make_shared<T>(args...);
        modulePtr->postConstructInitialize();
        size_t hash = std::hash<std::string>{}(modulePtr->name);
        moduleMap[hash] = modulePtr;
        ModuleManager::cguiRefresh = true;
    }

    template<typename T, typename... ArgsT>
std::shared_ptr<T> makeModule(ArgsT... args) {
        return std::make_shared<T>(args...);
    }


    template<typename T, typename... ArgsT>
    void addService(ArgsT... args) {
        auto servicePtr = std::make_shared<T>(args...);
        services.emplace_back(servicePtr);
    }

    void getModules();

    void syncState();

    bool doesAnyModuleHave(const std::string& settingName);
    void updateModulesVector();
    std::shared_ptr<Module> getModule(const std::string& name);

    inline std::map<size_t, std::shared_ptr<Module>> moduleMap;
    inline std::vector<std::shared_ptr<Listener>> services;
    inline std::vector<std::shared_ptr<Module>> modulesVector;
    inline bool initialized = false;
    inline bool restartModules = false;
    inline bool cguiRefresh = false;
}

