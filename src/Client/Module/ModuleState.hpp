#pragma once

/**
 * Lightweight module state flags extracted from ModuleManager.
 * Include this instead of Manager.hpp when you only need state flags.
 * This breaks the circular dependency: EventManager -> Manager -> Module -> EventManager
 */
namespace ModuleState {
    inline bool initialized = false;
    inline bool restartModules = false;
    inline bool cguiRefresh = false;
}
