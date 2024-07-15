#pragma once

#include "UIComponent.hpp"

class UIControl {
public:

    char filling[192]; //0x0000
    std::vector<std::unique_ptr<UIComponent>> components; //0xC0

    BUILD_ACCESS(this, std::string, LayerName, GET_OFFSET("UIControl::LayerName"));
    BUILD_ACCESS(this, float, scale, GET_OFFSET("UIControl::scale"));
    BUILD_ACCESS(this, float, x, GET_OFFSET("UIControl::x"));
    BUILD_ACCESS(this, float, y, GET_OFFSET("UIControl::y"));
    BUILD_ACCESS(this, std::vector<std::shared_ptr<UIControl>>, children, GET_OFFSET("UIControl::children"));

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