#pragma once

#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>

class Module;

// Version requirements for conditional module loading
struct VersionRequirement {
    enum class Type {
        None,           // No version check
        Above,          // Version must be above
        AboveOrEqual,   // Version must be above or equal
        Below,          // Version must be below
        BelowOrEqual,   // Version must be below or equal
        Between         // Version must be between two values
    };

    Type type = Type::None;
    int majorMin = 0;
    int minorMin = 0;
    int majorMax = 0;
    int minorMax = 0;

    static VersionRequirement none() { return {}; }
    static VersionRequirement above(int major, int minor) {
        return {Type::Above, major, minor, 0, 0};
    }
    static VersionRequirement aboveOrEqual(int major, int minor) {
        return {Type::AboveOrEqual, major, minor, 0, 0};
    }
    static VersionRequirement below(int major, int minor) {
        return {Type::Below, major, minor, 0, 0};
    }
    static VersionRequirement belowOrEqual(int major, int minor) {
        return {Type::BelowOrEqual, major, minor, 0, 0};
    }
    static VersionRequirement between(int majorMin, int minorMin, int majorMax, int minorMax) {
        return {Type::Between, majorMin, minorMin, majorMax, minorMax};
    }

    [[nodiscard]] bool isSatisfied() const;
};

// Module factory entry
struct ModuleFactoryEntry {
    std::string name;
    std::function<std::shared_ptr<Module>()> factory;
    VersionRequirement versionReq;
    int priority = 0; // Lower priority loads first
};

// Service factory entry
struct ServiceFactoryEntry {
    std::string name;
    std::function<std::shared_ptr<class Listener>()> factory;
    VersionRequirement versionReq;
};

class ModuleRegistry {
public:
    static ModuleRegistry& instance();

    // Register a module factory
    void registerModule(const std::string& name,
                        std::function<std::shared_ptr<Module>()> factory,
                        VersionRequirement versionReq = VersionRequirement::none(),
                        int priority = 0);

    // Register a service factory
    void registerService(const std::string& name,
                         std::function<std::shared_ptr<class Listener>()> factory,
                         VersionRequirement versionReq = VersionRequirement::none());

    // Get all registered module factories (filtered by version)
    [[nodiscard]] std::vector<ModuleFactoryEntry> getModuleFactories() const;

    // Get all registered service factories (filtered by version)
    [[nodiscard]] std::vector<ServiceFactoryEntry> getServiceFactories() const;

    // Create a specific module by name
    [[nodiscard]] std::shared_ptr<Module> createModule(const std::string& name) const;

    // Check if a module is registered
    [[nodiscard]] bool hasModule(const std::string& name) const;

    // Delete copy/move
    ModuleRegistry(const ModuleRegistry&) = delete;
    ModuleRegistry& operator=(const ModuleRegistry&) = delete;
    ModuleRegistry(ModuleRegistry&&) = delete;
    ModuleRegistry& operator=(ModuleRegistry&&) = delete;

private:
    ModuleRegistry() = default;
    ~ModuleRegistry() = default;

    std::vector<ModuleFactoryEntry> moduleFactories_;
    std::vector<ServiceFactoryEntry> serviceFactories_;
    std::unordered_map<std::string, size_t> moduleNameIndex_;
};

// Helper class for static registration
template<typename T>
class ModuleRegistrar {
public:
    ModuleRegistrar(const char* name,
                    VersionRequirement versionReq = VersionRequirement::none(),
                    int priority = 0) {
        ModuleRegistry::instance().registerModule(
            name,
            []() { return std::make_shared<T>(); },
            versionReq,
            priority
        );
    }
};

template<typename T>
class ServiceRegistrar {
public:
    ServiceRegistrar(const char* name,
                     VersionRequirement versionReq = VersionRequirement::none()) {
        ModuleRegistry::instance().registerService(
            name,
            []() { return std::make_shared<T>(); },
            versionReq
        );
    }
};

// Macros for easy registration
// Usage: REGISTER_MODULE(FPS) - registers with no version requirement
// Usage: REGISTER_MODULE_VERSION(MotionBlur, VersionRequirement::below(21, 120))
#define REGISTER_MODULE(ModuleClass) \
    static ModuleRegistrar<ModuleClass> _registrar_##ModuleClass(#ModuleClass)

#define REGISTER_MODULE_VERSION(ModuleClass, VersionReq) \
    static ModuleRegistrar<ModuleClass> _registrar_##ModuleClass(#ModuleClass, VersionReq)

#define REGISTER_MODULE_PRIORITY(ModuleClass, Priority) \
    static ModuleRegistrar<ModuleClass> _registrar_##ModuleClass(#ModuleClass, VersionRequirement::none(), Priority)

#define REGISTER_SERVICE(ServiceClass) \
    static ServiceRegistrar<ServiceClass> _registrar_##ServiceClass(#ServiceClass)

#define REGISTER_SERVICE_VERSION(ServiceClass, VersionReq) \
    static ServiceRegistrar<ServiceClass> _registrar_##ServiceClass(#ServiceClass, VersionReq)
