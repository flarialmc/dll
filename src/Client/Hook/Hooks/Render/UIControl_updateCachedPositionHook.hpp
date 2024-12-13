#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class UIControl_updateCachedPositionHook : public Hook {
private:
    static Vec2<float>* UIControl_getPosition(UIControl *_this) {
        static auto setCachedPosition = reinterpret_cast<decltype(&UIControl_getPosition)>(funcOriginal);

        auto* res = setCachedPosition(_this);

        auto event = nes::make_holder<UIControlGetPositionEvent>(_this, res);
        eventMgr.trigger(event);

        return res;
    }

    static void UIControl_updateCachedPosition21_30(UIControl *_this) {
        static auto setCachedPosition = reinterpret_cast<decltype(&UIControl_updateCachedPosition21_30)>(funcOriginal);

        setCachedPosition(_this);

        auto event = nes::make_holder<UIControlGetPositionEvent>(_this, nullptr);
        eventMgr.trigger(event);

        return;
    }

public:
    static inline void* funcOriginal = nullptr;

    UIControl_updateCachedPositionHook() : Hook("UIControl_updateCachedPositionHook", VersionUtils::checkAboveOrEqual(21,40) ? GET_SIG_ADDRESS("UIControl::getPosition") : GET_SIG_ADDRESS("UIControl::_setCachedPosition")) {}

    void enableHook() override {
        if(VersionUtils::checkAboveOrEqual(21, 40)) {
            this->autoHook((void *) UIControl_getPosition, (void **) &funcOriginal);
        } else {
            this->autoHook((void *) UIControl_updateCachedPosition21_30, (void **) &funcOriginal);
        }
    }
};