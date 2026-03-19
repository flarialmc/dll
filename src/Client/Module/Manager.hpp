#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include "Modules/Module.hpp"
#include "ModuleState.hpp"
#include "../Events/Listener.hpp"

// TODO make moduleMap AND modules but use moduleMap for search
namespace ModuleManager {
    extern std::map<size_t, std::shared_ptr<Module>> moduleMap;
    extern std::vector<std::shared_ptr<Listener>> services;

    // Queue for deferred enable/disable to avoid deadlock when called from event callbacks
    struct PendingToggle {
        std::shared_ptr<Module> module;
        bool enable;
    };
    inline std::queue<PendingToggle> pendingToggles;
    inline std::mutex pendingTogglesMutex;

    // Queue a module enable/disable to be processed outside of event callbacks
    inline void queueToggle(std::shared_ptr<Module> mod, bool enable) {
        if (!mod) return; // Ignore null modules
        std::lock_guard<std::mutex> lock(pendingTogglesMutex);
        pendingToggles.push({mod, enable});
    }

    // Process all pending toggles - call this at a safe point (not inside event callbacks)
    inline void processPendingToggles() {
        std::lock_guard<std::mutex> lock(pendingTogglesMutex);
        while (!pendingToggles.empty()) {
            auto& toggle = pendingToggles.front();
            // Safety check: ensure module is still valid
            if (toggle.module) {
                if (toggle.enable) {
                    toggle.module->onEnable();
                } else {
                    toggle.module->onDisable();
                }
                toggle.module->enabledState = toggle.enable;
                toggle.module->settings.getSettingByName<bool>("enabled")->value = toggle.enable;
            }
            pendingToggles.pop();
        }
    }

    void initialize();
    void restart();
    void terminate();

    /// Creates, initializes, and registers a module by type; triggers GUI refresh.
    template<typename T, typename... ArgsT>
    void addModule(ArgsT... args) {
        auto modulePtr = std::make_shared<T>(args...);
        modulePtr->postConstructInitialize();
        size_t hash = std::hash<std::string>{}(modulePtr->name);
        moduleMap[hash] = modulePtr;
        ModuleState::cguiRefresh = true;
    }

    /// Factory that creates a module instance without registering it in the module map.
    template<typename T, typename... ArgsT>
std::shared_ptr<T> makeModule(ArgsT... args) {
        return std::make_shared<T>(args...);
    }

    /// Registers a listener service that receives events without being a full module.
    template<typename T, typename... ArgsT>
    void addService(ArgsT... args) {
        auto servicePtr = std::make_shared<T>(args...);
        services.emplace_back(servicePtr);
    }

    void getModules();

    void syncState();

    /// Checks if any registered module has a setting with the given name.
    bool doesAnyModuleHave(const std::string& settingName);
    void updateModulesVector();
    /// Retrieves a registered module by its display name; returns nullptr if not found.
    std::shared_ptr<Module> getModule(const std::string& name);

    inline std::map<size_t, std::shared_ptr<Module>> moduleMap;
    inline std::vector<std::shared_ptr<Listener>> services;
    inline std::vector<std::shared_ptr<Module>> modulesVector;

    // Aliases to ModuleState for backwards compatibility
    inline bool& initialized = ModuleState::initialized;
    inline bool& restartModules = ModuleState::restartModules;
    inline bool& cguiRefresh = ModuleState::cguiRefresh;
}

