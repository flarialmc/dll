#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class _composeFullStackHook : public Hook {
private:

    static void ResourcePackManager__composeFullStack(_composeFullStackHook* _this) {
        if (resourcePackManager == nullptr) {
            resourcePackManager = _this;
            auto vTable = *(uintptr_t **) _this;
            Logger::custom(fg(fmt::color::dodger_blue), "vFunc Hook", "Hooked resourcePackManager at {}", (void*)resourcePackManager);
            resourcePackManagerLoad = reinterpret_cast<PFN_ResourcePackManager_load>(*(vTable + 3));
            Logger::custom(fg(fmt::color::dodger_blue), "vFunc Hook", "Hooked resourcePackManagerLoad at {}", (void*)resourcePackManagerLoad);
        }
        auto event = nes::make_holder<PacksLoadEvent>();
        eventMgr.trigger(event);

        funcOriginal(_this);
    }

public:
    typedef void(__thiscall *original)(_composeFullStackHook*);
    static inline _composeFullStackHook* resourcePackManager = nullptr;
    typedef bool(__thiscall *PFN_ResourcePackManager_load)(void *This, const ResourceLocation &location, std::string &resourceStream);
    static inline PFN_ResourcePackManager_load resourcePackManagerLoad = nullptr;
    static inline original funcOriginal = nullptr;

    _composeFullStackHook() : Hook("_composeFullStackHook", GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack")) {}

    void enableHook() override {
        this->autoHook((void *) ResourcePackManager__composeFullStack, (void **) &funcOriginal);
    }
};