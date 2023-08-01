#pragma once

#include "UIComponent.hpp"

class UIControl {
public:

    char filling[192]; //0x0000
    std::vector<std::unique_ptr<UIComponent>> components; //0xC0

    BUILD_ACCESS(this, std::string, LayerName, 0x18);
    BUILD_ACCESS(this, float, scale, 0x40);
    BUILD_ACCESS(this, float, x, 0x78);
    BUILD_ACCESS(this, float, y, 0x7C);
    BUILD_ACCESS(this, std::vector<std::shared_ptr<UIControl>>, children, 0xA0);

    void getAllControls(std::vector<std::shared_ptr<UIControl>>& list) {
        for (auto& control : this->children) {
            list.emplace_back(control);
            control->getAllControls(list);
        }
    }

    void forEachControl(std::function<void(std::shared_ptr<UIControl>&)> func) {

        std::vector<std::shared_ptr<UIControl>> writeList;
        this->getAllControls(writeList);

        for (auto& control : writeList) {
            func(control);
        }
    }
};