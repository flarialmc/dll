#pragma once

#include "../Module.hpp"
#include "../../../../Assets/Assets.hpp"


class RawInputBuffer : public Module {

public:

    RawInputBuffer(): Module("Raw Input Buffer", "Fixes Minecraft's default input delay",
        IDR_CURSOR_PNG, "", false, {"input lag"}) {

    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;
};
