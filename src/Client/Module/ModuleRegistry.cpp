#include "ModuleRegistry.hpp"
#include <Utils/VersionUtils.hpp>
#include <algorithm>

bool VersionRequirement::isSatisfied() const {
    switch (type) {
        case Type::None:
            return true;
        case Type::Above:
            return VersionUtils::checkAbove(majorMin, minorMin);
        case Type::AboveOrEqual:
            return VersionUtils::checkAboveOrEqual(majorMin, minorMin);
        case Type::Below:
            return VersionUtils::checkBelow(majorMin, minorMin);
        case Type::BelowOrEqual:
            return VersionUtils::checkBelowOrEqual(majorMin, minorMin);
        case Type::Between:
            return VersionUtils::checkAboveOrEqual(majorMin, minorMin) &&
                   VersionUtils::checkBelowOrEqual(majorMax, minorMax);
    }
    return false;
}

ModuleRegistry& ModuleRegistry::instance() {
    static ModuleRegistry instance;
    return instance;
}

void ModuleRegistry::registerModule(const std::string& name,
                                     std::function<std::shared_ptr<Module>()> factory,
                                     VersionRequirement versionReq,
                                     int priority) {
    moduleNameIndex_[name] = moduleFactories_.size();
    moduleFactories_.push_back({name, std::move(factory), versionReq, priority});
}

void ModuleRegistry::registerService(const std::string& name,
                                      std::function<std::shared_ptr<Listener>()> factory,
                                      VersionRequirement versionReq) {
    serviceFactories_.push_back({name, std::move(factory), versionReq});
}

std::vector<ModuleFactoryEntry> ModuleRegistry::getModuleFactories() const {
    std::vector<ModuleFactoryEntry> result;
    result.reserve(moduleFactories_.size());

    for (const auto& entry : moduleFactories_) {
        if (entry.versionReq.isSatisfied()) {
            result.push_back(entry);
        }
    }

    // Sort by priority
    std::sort(result.begin(), result.end(),
              [](const ModuleFactoryEntry& a, const ModuleFactoryEntry& b) {
                  return a.priority < b.priority;
              });

    return result;
}

std::vector<ServiceFactoryEntry> ModuleRegistry::getServiceFactories() const {
    std::vector<ServiceFactoryEntry> result;
    result.reserve(serviceFactories_.size());

    for (const auto& entry : serviceFactories_) {
        if (entry.versionReq.isSatisfied()) {
            result.push_back(entry);
        }
    }

    return result;
}

std::shared_ptr<Module> ModuleRegistry::createModule(const std::string& name) const {
    auto it = moduleNameIndex_.find(name);
    if (it == moduleNameIndex_.end()) {
        return nullptr;
    }

    const auto& entry = moduleFactories_[it->second];
    if (!entry.versionReq.isSatisfied()) {
        return nullptr;
    }

    return entry.factory();
}

bool ModuleRegistry::hasModule(const std::string& name) const {
    return moduleNameIndex_.find(name) != moduleNameIndex_.end();
}
