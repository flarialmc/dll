#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class _composeFullStackHook : public Hook {
private:
    static void ResourcePackManager__composeFullStack(class ResourcePackManager* _this) {

        auto event = nes::make_holder<PacksLoadEvent>();
        eventMgr.trigger(event);

        funcOriginal(_this);
    }

public:
    typedef void(__thiscall *original)(ResourcePackManager*);

    static inline original funcOriginal = nullptr;

    _composeFullStackHook() : Hook("_composeFullStackHook", GET_SIG_ADDRESS("ResourcePackManager::_composeFullStack")) {}

    void enableHook() override {
        this->autoHook((void *) ResourcePackManager__composeFullStack, (void **) &funcOriginal);
    }
};