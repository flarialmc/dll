#include "CPSCounter.hpp"

#include "Client.hpp"
#include <Modules/Misc/Input/GUIMouseListener.hpp>
#include "GUI/Engine/ExpressionFormat.hpp"


void CPSCounter::onEnable() {
    Listen(this, RenderEvent, &CPSCounter::onRender)
    Module::onEnable();
}

void CPSCounter::onDisable() {
    Deafen(this, RenderEvent, &CPSCounter::onRender)
    Module::onDisable();
}

void CPSCounter::defaultConfig() {
    setDef("text", (std::string)"CPS: {lmb} | {rmb}");
    Module::defaultConfig("all");
    
}

void CPSCounter::settingsRender(float settingsOffset) {

    initSettingsPage();


    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    addTextBox("Format",
            "Use {LMB} and {RMB} for left and right CPS.\n"
            "Colors: {red}, {green}, {blue}, {yellow}, {#ff8800}\n"
            "Dynamic: {colorRangeInverse(value, 30, 60)} for FPS-like\n"
            "Math: {percent(value, max)}, {round(value, 2)}, {clamp(value, 0, 100)}\n"
            "Conditionals: {if(value, 50, GOOD, LOW)}, {ifgt}, {iflt}",
            0, "text");
    addSlider("Text Scale", "", "textscale", 2.0f);
    addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, "textalignment", true);
    addToggle("Text Shadow", "Displays a shadow under the text", "textShadow");
    addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}


void CPSCounter::onRender(RenderEvent& event)  {

    if (!isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;

    std::string leftCPS = FlarialGUI::cached_to_string(GUIMouseListener::GetLeftCPS());
    std::string rightCPS = FlarialGUI::cached_to_string(GUIMouseListener::GetRightCPS());

    auto textSetting = this->settings.getSettingByName<std::string>("text");

    std::string text{};
    if (textSetting != nullptr) text = getOps<std::string>("text");

    std::string uppercaseSentence;
    for (char c: text) {
        uppercaseSentence += (char) std::toupper(c);
    }

    if (!first) {
        if (uppercaseSentence.find("{VALUE}") != std::string::npos) {
            textSetting->value = (std::string)"CPS: {lmb} | {rmb}";
            Client::SaveSettings();
            FlarialGUI::Notify("Your CPS Counter text was changed due to an update.");
        }
        first = true;
    }

    size_t pos = uppercaseSentence.find("{LMB}");
    if (pos != std::string::npos) {
        text.replace(pos, 5, leftCPS);
    }

    uppercaseSentence = "";
    for (char c: text) {
        uppercaseSentence += (char) std::toupper(c);
    }

    size_t pos1 = uppercaseSentence.find("{RMB}");
    if (pos1 != std::string::npos) {
        text.replace(pos1, 5, rightCPS);
    }

    normalRenderCore(1, text);
}
