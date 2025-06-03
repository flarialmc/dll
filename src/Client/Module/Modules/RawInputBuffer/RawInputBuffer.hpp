#pragma once

#include "../Module.hpp"


class RawInputBuffer : public Module {
public:
    RawInputBuffer() : Module("Raw Input Buffer", "Fixes Minecraft's default input delay", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Module::onEnable();
    }

    void onDisable() override {
        Module::onDisable();
    }

    void defaultConfig() override {
        Module::defaultConfig("core");
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));
        addHeader("troll");
        std::string troll("troll");
        addDropdown("troll", "the voices", std::vector<std::string>{ "troll", "troll", "troll even more", "most troll"}, troll);
        FlarialGUI::UnsetScrollView();

        resetPadding();
    }
};
