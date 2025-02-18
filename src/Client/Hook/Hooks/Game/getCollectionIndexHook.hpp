#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class getCollectionIndexHook : public Hook {
private:
//    static int64_t CourseScreenController__getCollectionIndex(class CourseScreenController *_this, class UIPropertyBag *bag) {
//
//        auto index = funcOriginal(_this, bag);
//
//        auto event = nes::make_holder<ContainerSlotHovered>(index);
//        eventMgr.trigger(event);
//
//        return index;
//    }

    static int64_t CourseScreenController__getCollectionIndex_someVfunc(void *_this, std::string* containerName, int64_t slot) {

        auto event = nes::make_holder<ContainerSlotHovered>(*containerName, slot);
        eventMgr.trigger(event);

        return funcOriginal(_this, containerName, slot);
    }

public:
//    typedef int64_t(__thiscall *original)(class CourseScreenController *, class UIPropertyBag *);
//
//    static inline original funcOriginal = nullptr;

    typedef int64_t(__thiscall *original)(void *, void *, int64_t);

    static inline original funcOriginal = nullptr;

    getCollectionIndexHook() : Hook("getCollectionIndexHook", GET_SIG_ADDRESS("CourseScreenController::_getCollectionIndex_someVfunc")) {}

    void enableHook() override {
        this->autoHook((void *) CourseScreenController__getCollectionIndex_someVfunc, (void **) &funcOriginal);
    }
};