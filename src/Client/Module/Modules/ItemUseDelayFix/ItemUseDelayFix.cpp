#include "ItemUseDelayFix.hpp"

void ItemUseDelayFix::onEnable() {
    patch();
    Module::onEnable();
}

void ItemUseDelayFix::onDisable() {
    unpatch();
    Module::onDisable();
}

void ItemUseDelayFix::defaultConfig() {
    Module::defaultConfig("core");
    
}

void ItemUseDelayFix::patch() {
    Memory::nopBytes((void *)address, original.size());
}

void ItemUseDelayFix::unpatch() {
    Memory::patchBytes((void *)address, original.data(), original.size());
}
