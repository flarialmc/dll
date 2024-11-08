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

    void getAllControls(std::vector<std::shared_ptr<UIControl>>& list) {
        for (auto& control : this->children) {
            list.emplace_back(control);
            control->getAllControls(list);
        }
    }

    void forEachControl(std::function<void(std::shared_ptr<UIControl>&)>&& func) {

        std::vector<std::shared_ptr<UIControl>> writeList;
        this->getAllControls(writeList);

        for (auto& control : writeList) {
            func(control);
        }
    }
};