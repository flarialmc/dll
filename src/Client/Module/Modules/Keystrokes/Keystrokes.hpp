#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"

#include <Windows.h>
#include "KeystrokesListener.hpp"
#include "../CPS/CPSListener.hpp"
#include "../../../Events/EventHandler.hpp"

class Keystrokes : public Module {

public:

    enum Strokes {
        W,
        A,
        S,
        D,
        SPACEBAR,
        LMB,
        RMB
    };

    std::vector<D2D1_COLOR_F> states;
    std::vector<D2D1_COLOR_F> textStates;


    Keystrokes() : Module("Keystrokes", "Displays real-time information about your\nWASD and mouse inputs.",
                          IDR_KEYBOARD_PNG, "") {
        Module::setup();
    };

    void onSetup() override {
        /*
        D2D1_COLOR_F d = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);
        D2D1_COLOR_F e = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textColor")->value);
        d.a = settings.getSettingByName<float>("bgOpacity")->value;
        e.a = settings.getSettingByName<float>("textOpacity")->value;

        states = std::vector<D2D1_COLOR_F>(7, d);
        textStates = std::vector<D2D1_COLOR_F>(7, e);
        */
    }

    void onEnable() override {
        EventHandler::registerListener(new KeystrokesListener("Keystrok", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Keystrok");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("rounding") == nullptr)
            settings.addSetting("rounding", 11.0f);

        if (settings.getSettingByName<bool>("cps") == nullptr)
            settings.addSetting("cps", false);

        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }

        if (settings.getSettingByName<bool>("border") == nullptr) {
            settings.addSetting("border", true);
            settings.addSetting("borderWidth", 1.0f);
        }

        if (settings.getSettingByName<std::string>("enabledColor") == nullptr)
            settings.addSetting("enabledColor", (std::string) "fafafa");
        if (settings.getSettingByName<bool>("enabledRGB") == nullptr)
            settings.addSetting("enabledRGB", false);
        if (settings.getSettingByName<float>("enabledRGBSpeed") == nullptr)
            settings.addSetting("enabledRGBSpeed", 1.0f);

        if (settings.getSettingByName<std::string>("textEnabledColor") == nullptr)
            settings.addSetting("textEnabledColor", (std::string) "fafafa");
        if (settings.getSettingByName<bool>("textEnabledRGB") == nullptr)
            settings.addSetting("textEnabledRGB", false);
        if (settings.getSettingByName<float>("textEnabledRGBSpeed") == nullptr)
            settings.addSetting("textEnabledRGBSpeed", 1.0f);

        if (settings.getSettingByName<float>("enabledOpacity") == nullptr)
            settings.addSetting("enabledOpacity", 0.55f);

        if (settings.getSettingByName<float>("textEnabledOpacity") == nullptr)
            settings.addSetting("textEnabledOpacity", 0.55f);

        if (settings.getSettingByName<float>("uiscale") == nullptr)
            settings.addSetting("uiscale", 1.0f);

        if (settings.getSettingByName<float>("textscale") == nullptr)
            settings.addSetting("textscale", 1.0f);

        if (settings.getSettingByName<float>("textscale2") == nullptr)
            settings.addSetting("textscale2", 1.0f);

        if (settings.getSettingByName<bool>("BlurEffect") == nullptr)
            settings.addSetting("BlurEffect", false);

        if (settings.getSettingByName<bool>("lmbrmb") == nullptr)
            settings.addSetting("lmbrmb", true);

        if (settings.getSettingByName<bool>("hidecps") == nullptr)
            settings.addSetting("hidecps", true);

        if (settings.getSettingByName<std::string>("lmbtext") == nullptr)
            settings.addSetting("lmbtext", (std::string) "{value} CPS");

        if (settings.getSettingByName<std::string>("rmbtext") == nullptr)
            settings.addSetting("rmbtext", (std::string) "{value} CPS");

        if (settings.getSettingByName<std::string>("wText") == nullptr)
            settings.addSetting("wText", (std::string) "W");

        if (settings.getSettingByName<std::string>("aText") == nullptr)
            settings.addSetting("aText", (std::string) "A");

        if (settings.getSettingByName<std::string>("sText") == nullptr)
            settings.addSetting("sText", (std::string) "S");

        if (settings.getSettingByName<std::string>("dText") == nullptr)
            settings.addSetting("dText", (std::string) "D");

        if (settings.getSettingByName<float>("spacebarWidth") == nullptr)
            settings.addSetting("spacebarWidth", 0.5f);

        if (settings.getSettingByName<float>("spacebarHeight") == nullptr) {
            settings.addSetting("spacebarHeight", 0.09f);
        }

        if (settings.getSettingByName<float>("keySpacing") == nullptr) {
            settings.addSetting("keySpacing", 1.63f);
        }
    }

    void settingsRender() override { }

    static std::pair<float, float>
    centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
        return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
    }
};
