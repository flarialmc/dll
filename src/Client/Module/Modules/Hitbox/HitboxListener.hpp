#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <vector>

class HitboxListener : public Listener {

    Module *module;
    static inline std::vector<AABB> aabbsToRender;
public:

    void onRender(RenderEvent &event) override;

    void onSetupAndRender(SetupAndRenderEvent &event) override;

    explicit HitboxListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
