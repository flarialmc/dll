#pragma once

#include "../Module.hpp"


class RawInputBuffer : public Module {
public:
    RawInputBuffer();;

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;
};
