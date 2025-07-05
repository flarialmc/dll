#pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../Manager.hpp"
#include "../../../Client.hpp"
#include "../Nick/NickModule.hpp"
#include <vector>
#include <algorithm>

class SkinStealer : public Module {

public:
    SkinStealer();;

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onKey(const KeyEvent& event);
};