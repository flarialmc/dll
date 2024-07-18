#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "BlockBreakIndicatorListener.hpp"
#include "../CPS/CPSListener.hpp"
#include "../../../Client.hpp"

class BlockBreakIndicator : public Module {

public:


    BlockBreakIndicator() : Module("Break Progress", "Visual indicator to show the progress\nof breaking a block.",
                                   IDR_BLOCK_BREAK_INDICATOR_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new BlockBreakIndicatorListener("BlockBreakIndicatorListener", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("BlockBreakIndicatorListener");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value}");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 1.00f);
        if (settings.getSettingByName<bool>("pbmode") == nullptr) settings.addSetting("pbmode", true);
        if (settings.getSettingByName<bool>("onlyShowWhileBreaking") == nullptr)
            settings.addSetting("onlyShowWhileBreaking", false);
        if (settings.getSettingByName<std::string>("orientation") == nullptr)
            settings.addSetting("orientation", (std::string) "Vertical");
        if (settings.getSettingByName<float>("pbwidth") == nullptr) settings.addSetting("pbwidth", 0.91f);
        if (settings.getSettingByName<float>("pbheight") == nullptr) settings.addSetting("pbheight", 0.82f);

        if (settings.getSettingByName<std::string>("barFill") == nullptr)
            settings.addSetting("barFill", (std::string) "a83232");
        if (settings.getSettingByName<float>("barFillOpacity") == nullptr) settings.addSetting("barFillOpacity", 1.0f);
        if (settings.getSettingByName<bool>("barFillRGB") == nullptr) settings.addSetting("barFillRGB", false);

    }

    void settingsRender() override { }

    float currentHeight = 0.0f;

    void normalRender() override { }
};

