#pragma once

#include <string>

/**
 * Minimal interface for modules to break circular dependencies.
 * This interface exposes only what external code needs to check module state.
 */
class IModule {
public:
    virtual ~IModule() = default;

    virtual bool isEnabled() const = 0;
    virtual bool isActive() const = 0;
    virtual const std::string& getName() const = 0;
};
