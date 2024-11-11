#pragma once

#include "UIComponent.hpp"

class UIControl {
public:
    BUILD_ACCESS(this, Vec2<float>, sizeConstrains, GET_OFFSET("UIControl::sizeConstrains"));
    BUILD_ACCESS(this, Vec2<float>, parentRelativePosition, GET_OFFSET("UIControl::parentRelativePosition"));
    BUILD_ACCESS(this, std::vector<std::shared_ptr<UIControl>>, children, GET_OFFSET("UIControl::children"));

    std::string& getLayerName() {
        return hat::member_at<std::string>(this, GET_OFFSET("UIControl::LayerName"));
    }

    std::vector<std::unique_ptr<UIComponent>>& getComponents() {
        return hat::member_at<std::vector<std::unique_ptr<UIComponent>>>(this, GET_OFFSET("UIControl::components"));
    }

    void updatePosition(bool override = false) {
        if(WinrtUtils::checkAboveOrEqual(21, 40)) {
            int& flags = hat::member_at<int>(this, 0x18);
            flags |= 1; // set cachedPositionDirty
            using func = Vec2<float>*(__fastcall*)(UIControl*);
            static auto getPosition = reinterpret_cast<func>(GET_SIG_ADDRESS("UIControl::getPosition"));
            if(override) {
                auto newPos = parentRelativePosition;
                auto* pos = getPosition(this);
                *pos = newPos;
            } else {
                getPosition(this);
            }
            return;
        } else {
            using func = void (__fastcall *)(UIControl *);
            static auto setCachedPosition = reinterpret_cast<func>(GET_SIG_ADDRESS("UIControl::_setCachedPosition"));
            return setCachedPosition(this);
        }
    }

    void getAllControls(std::vector<std::shared_ptr<UIControl>>& list) {
        for (auto& control : this->children) {
            list.emplace_back(control);
            control->getAllControls(list);
        }
    }

    void forEachControl(std::function<bool(std::shared_ptr<UIControl>&)>&& func) {
        std::vector<std::shared_ptr<UIControl>> writeList;
        this->getAllControls(writeList);

        for (auto& control : writeList) {
            bool res = func(control);
            if(res) return;
        }
    }

    void forEachChild(std::function<void(std::shared_ptr<UIControl>&)> func) {
        for (auto& control : children) {
            func(control);
            control->forEachChild(func);
        }
    }
};