#include "Keystrokes.hpp"

#include "Events/EventManager.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Modules/CPS/CPSCounter.hpp"
#include <Modules/Misc/Input/GUIMouseListener.hpp>

void Keystrokes::onSetup() {
    defaultConfig();
    D2D1_COLOR_F d = getColor("bg");
    D2D1_COLOR_F e = getColor("text");

    states = std::vector<D2D1_COLOR_F>(7, d);
    textStates = std::vector<D2D1_COLOR_F>(7, e);
    shadowStates = std::vector<D2D1_COLOR_F>(7, e);
}

void Keystrokes::onEnable() {
    Listen(this, RenderEvent, &Keystrokes::onRender)
    Module::onEnable();
}

void Keystrokes::onDisable() {
    Deafen(this, RenderEvent, &Keystrokes::onRender)
    Module::onDisable();
}

void Keystrokes::defaultConfig() {
    settings.renameSetting("glowEnabledColor", "glowEnabledOpacity", "glowEnabledRGB", "glowEnabled");
    settings.renameSetting("enabledColor", "enabledOpacity", "enabledRGB", "enabled");
    settings.renameSetting("textEnabledColor", "textEnabledOpacity", "textEnabledRGB", "textEnabled");

    setDef("rounding", 11.0f);
    setDef("cps", false);
    setDef("border", false);
    setDef("borderWidth", 1.0f);

    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    Module::defaultConfig("main");
    Module::defaultConfig("colors");

    setDef("glowEnabled", (std::string) "F0F0F0", 1.f, false);
    setDef("enabled", (std::string) "fafafa", 0.55f, false);
    setDef("EtextShadow", (std::string) "00000", 0.55f, false);
    setDef("DtextShadow", (std::string) "00000", 0.55f, false);
    setDef("textEnabled", (std::string) "fafafa", 1.f, false);
    setDef("textShadow", false);
    setDef("textShadowOffset", 0.003f);
    setDef("textEnabledRGBSpeed", 1.0f);
    setDef("enabledOpacity", 0.55f);
    setDef("uiscale", 1.0f);
    setDef("textscale", 1.0f);
    setDef("textscale2", 1.0f);
    setDef("BlurEffect", false);
    setDef("lmbrmb", true);
    setDef("hidecps", true);
    setDef("mainlmbtext", (std::string) "LMB");
    setDef("mainrmbtext", (std::string) "RMB");
    setDef("lmbtext", (std::string) "{value} CPS");
    setDef("rmbtext", (std::string) "{value} CPS");
    setDef("wText", (std::string) "W");
    setDef("aText", (std::string) "A");
    setDef("sText", (std::string) "S");
    setDef("dText", (std::string) "D");
    setDef("spacebarWidth", 0.5f);
    setDef("spacebarHeight", 0.09f);
    setDef("keySpacing", 1.63f);
    setDef("edSpeed", 1.f);
    setDef("glowEnabledAmount", 50.f);
    setDef("glowSpeed", 1.f);
    setDef("glowEnabled", false);
    setDef("textXOffset", 0.5f);
    setDef("textYOffset", 0.5f);
}

void Keystrokes::settingsRender(float settingsOffset) {
    initSettingsPage();


    addHeader("Keystrokes");
    addSlider("UI Scale", "", "uiscale", 2.0f);
    addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", "BlurEffect");
    addSlider("Rounding", "Rounding of the rectangle", "rounding");
    addToggle("Background", "", "showBg");
    addConditionalToggle(getOps<bool>("showBg"), "Background Shadow", "Displays a shadow under the background", "rectShadow");
    addConditionalSlider(getOps<bool>("showBg") && getOps<bool>("rectShadow"), "Shadow Offset", "How far the shadow will be.", "rectShadowOffset", 0.02f, 0.001f);
    addToggle("Border", "", "border");
    addConditionalSlider(getOps<bool>("border"), "Border Thickness", "", "borderWidth", 4.f);
    addToggle("Glow (Disabled State)", "", "glow");
    addConditionalSlider(getOps<bool>("glow"), "Glow Amount (Disabled State)", "", "glowAmount", 100.f);
    addToggle("Glow (Enabled State)", "", "glowEnabled");
    addConditionalSlider(getOps<bool>("glowEnabled"), "Glow Amount (Enabled State)", "", "glowEnabledAmount", 100.f);
    addConditionalSlider(getOps<bool>("glowEnabled"), "Glow Speed", "", "glowSpeed", 10.f);

    addSlider("Key Spacing", "", "keySpacing", 10.00);
    addSlider("Spacebar Width", "", "spacebarWidth", 1.00, 0, false);
    addSlider("Spacebar Height", "", "spacebarHeight", 1.00, 0, false);
    addSlider("Highlight Speed", "", "edSpeed", 10.f);
    extraPadding();

    addHeader("Text");
    addSlider("X Offset", "", "textXOffset", 1.f);
    addSlider("Y Offset", "", "textYOffset", 1.f);
    addToggle("Text Shadow", "Displays a shadow under the text", "textShadow");
    addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Mouse Buttons");
    addToggle("Show Mouse Buttons", "LMB & RMB", "cps");
    addConditionalSlider(getOps<bool>("cps"), "CPS Text Scale", "", "textscale2", 2.00);
    addToggle("Show LMB & RMB", "", "lmbrmb");
    addToggle("Hide CPS Counter", "", "hidecps");
    addTextBox("LMB Text", "", 0, "mainlmbtext");
    addTextBox("RMB Text", "", 0, "mainrmbtext");
    addTextBox("LMB CPS Text", "", 0, "lmbtext");
    addTextBox("RMB CPS Text", "", 0, "rmbtext");
    extraPadding();

    addHeader("WASD");
    addSlider("WASD Text Scale", "", "textscale", 2.00);
    addTextBox("W Key", "", 0, "wText");
    addTextBox("A Key", "", 0, "aText");
    addTextBox("S Key", "", 0, "sText");
    addTextBox("D Key", "", 0, "dText");
    extraPadding();

    addHeader("Colors");
    addColorPicker("Background Disabled", "", "bg");
    addColorPicker("Background Enabled", "", "enabled");
    addColorPicker("Text Disabled", "", "text");
    addColorPicker("Text Enabled", "", "textEnabled");
    addConditionalColorPicker(getOps<bool>("rectShadow"), "Background Shadow Color", "", "rectShadow");
    addConditionalColorPicker(getOps<bool>("textShadow"), "Text Shadow Color (Disabled State)", "", "DtextShadow");
    addConditionalColorPicker(getOps<bool>("textShadow"), "Text Shadow Color (Enabled State)", "", "EtextShadow");
    addColorPicker("Border Color", "", "border");
    addColorPicker("Glow Color (Disabled State)", "", "glow");
    addColorPicker("Glow Color (Enabled State)", "", "glowEnabled");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

std::pair<float, float> Keystrokes::centerChildRectangle(float parentWidth, float parentHeight, float childWidth,
                                                         float childHeight) {
    return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
}

void Keystrokes::normalRender(int index, std::string &value) {
    if (!this->isEnabled()) return;
    if (SDK::hasInstanced) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            bool enableGlow = getOps<bool>("glowEnabled");
            bool enableTextShadow = getOps<bool>("textShadow");
            bool bgGlow = getOps<bool>("glow");
            bool hideCPS = getOps<bool>("hidecps");
            bool smb = getOps<bool>("cps");
            bool lmbrmb = getOps<bool>("lmbrmb");

            float textShadowOffset = Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");

            D2D1_COLOR_F borderColor = getColor("border");
            D2D1_COLOR_F glowEnabledColor = getColor("glowEnabled");
            D2D1_COLOR_F glowColor = getColor("glow");

            // lmb
            std::string lmbText = getOps<std::string>("lmbtext");
            std::string uppercaseSentence;
            std::string search = "{VALUE}";

            for (char c: lmbText) uppercaseSentence += (char) std::toupper(c);

            size_t pos = uppercaseSentence.find(search);
            if (pos != std::string::npos) {
                lmbText.replace(pos, search.length(), FlarialGUI::cached_to_string(GUIMouseListener::GetLeftCPS()));
            }

            // rmb
            std::string rmbText = getOps<std::string>("rmbtext");
            uppercaseSentence = "";
            for (char c: rmbText) uppercaseSentence += (char) std::toupper(c);

            pos = uppercaseSentence.find(search);
            if (pos != std::string::npos) {
                rmbText.replace(pos, search.length(), FlarialGUI::cached_to_string(GUIMouseListener::GetRightCPS()));
            }

            float keycardSize = Constraints::RelativeConstraint(0.08f * getOps<float>("uiscale"), "height", true);
            float spacing = getOps<float>("keySpacing") * Constraints::RelativeConstraint(0.0015f * getOps<float>("uiscale"), "height", true);
            float textSize = getOps<float>("textscale");
            float textSize2 = getOps<float>("textscale2");

            Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));

            float totalWidth = keycardSize * 3 + spacing * 2;
            float totalHeight = keycardSize * 2.0f + keycardSize / 2.0f + spacing * 2;
            if (getOps<bool>("cps")) totalHeight += (keycardSize + spacing);

            Vec2<float> realcenter;

            if (settingperc.x != 0) realcenter = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
            else realcenter = Constraints::CenterConstraint(keycardSize, keycardSize);

            Vec2<float> rounde = Constraints::RoundingConstraint(
                getOps<float>("rounding") *
                getOps<float>("uiscale"),
                getOps<float>("rounding") *
                getOps<float>("uiscale"));


            if (ClickGUI::editmenu) {
                // makes module movable
                FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, totalWidth, totalHeight, index, this->name);
                checkForRightClickAndOpenSettings(realcenter.x, realcenter.y, totalWidth, totalHeight);

                Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, totalWidth, totalHeight);

                realcenter.x = vec2.x;
                realcenter.y = vec2.y;

                realcenter = realcenter;

                Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y, 0.f, 0.f);
                this->settings.setValue("percentageX", percentages.x);
                this->settings.setValue("percentageY", percentages.y);
            }

            float fontSize = Constraints::SpacingConstraint(textSize * 3.0f, keycardSize);
            float fontSize2 = Constraints::SpacingConstraint(textSize2 * 3.0f, keycardSize);

            //todo only resave colors on settings render and once on setup

            D2D1_COLOR_F enabledColor = getColor("enabled");
            D2D1_COLOR_F disabledColor = getColor("bg");
            D2D1_COLOR_F textColor = getColor("text");
            D2D1_COLOR_F EtextShadowColor = getColor("EtextShadow");
            D2D1_COLOR_F DtextShadowColor = getColor("DtextShadow");
            D2D1_COLOR_F textEnabledColor = getColor("textEnabled");

            MoveInputComponent *handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

            std::vector<bool> handlerRes = {
                handler->mInputState.forward,
                handler->mInputState.left,
                handler->mInputState.backward,
                handler->mInputState.right,
                MC::heldLeft,
                MC::heldRight,
                handler->jumping
            };

            for (size_t i = 0; i < handlerRes.size(); i++) {
                if (handlerRes[i]) {
                    states[i] = FlarialGUI::LerpColor(states[i], enabledColor, 0.15f * FlarialGUI::frameFactor * getOps<float>("edSpeed"));
                    textStates[i] = FlarialGUI::LerpColor(textStates[i], textEnabledColor, 0.15f * FlarialGUI::frameFactor * getOps<float>("edSpeed"));
                    if (enableTextShadow) shadowStates[i] = FlarialGUI::LerpColor(shadowStates[i], EtextShadowColor, 0.15f * FlarialGUI::frameFactor * getOps<float>("edSpeed"));
                    if (enableGlow) FlarialGUI::lerp(glowAmountModifier[i], 1.f, FlarialGUI::frameFactor * getOps<float>("glowSpeed"));
                } else {
                    states[i] = FlarialGUI::LerpColor(states[i], disabledColor, 0.15f * FlarialGUI::frameFactor * getOps<float>("edSpeed"));
                    textStates[i] = FlarialGUI::LerpColor(textStates[i], textColor, 0.15f * FlarialGUI::frameFactor * getOps<float>("edSpeed"));
                    if (enableTextShadow) shadowStates[i] = FlarialGUI::LerpColor(shadowStates[i], DtextShadowColor, 0.15f * FlarialGUI::frameFactor * getOps<float>("edSpeed"));
                    if (enableGlow) FlarialGUI::lerp(glowAmountModifier[i], 0.f, FlarialGUI::frameFactor * getOps<float>("glowSpeed"));
                }
            }

            Vec2<float> b_realcenter = realcenter;

            std::vector<Vec2<float> > positions = {
                Vec2<float>(realcenter.x + keycardSize + spacing, realcenter.y), // W
                Vec2<float>(realcenter.x, realcenter.y + keycardSize + spacing), // A
                Vec2<float>(realcenter.x + keycardSize + spacing, realcenter.y + keycardSize + spacing), // S
                Vec2<float>(realcenter.x + (keycardSize + spacing) * 2.f, realcenter.y + keycardSize + spacing), // D
                Vec2<float>(realcenter.x, realcenter.y + (keycardSize + spacing) * 2.f), // LMB
                Vec2<float>(realcenter.x + (keycardSize + spacing) * 1.5f, realcenter.y + (keycardSize + spacing) * 2.f), // RMB
                Vec2<float>(realcenter.x, smb ? (realcenter.y + (keycardSize + spacing) * 3.f - (keycardSize * 0.05f)) : (realcenter.y + (keycardSize + spacing) * 2.f)) // SPACEBAR
            };

            Vec2<float> textOffset = Vec2<float>{
                Constraints::RelativeConstraint(getOps<float>("textXOffset") - 0.5f) * getOps<float>("uiscale") * 0.05f,
                Constraints::RelativeConstraint(getOps<float>("textYOffset") - 0.5f) * getOps<float>("uiscale") * 0.05f
            };

            std::vector<std::string> opString = {
                getOps<std::string>("wText"),
                getOps<std::string>("aText"),
                getOps<std::string>("sText"),
                getOps<std::string>("dText"),
                getOps<std::string>("mainlmbtext"),
                getOps<std::string>("mainrmbtext")
            };

            for (size_t i = 0; i < positions.size(); i++) {
                if (!smb && (i == 4 || i == 5)) continue;

                Vec2<float> keycardDims = (i == 4 || i == 5) ? Vec2<float>(keycardSize + (keycardSize / 2.0f) + spacing / 2.0f, keycardSize - (keycardSize * 0.05f)) : (i == 6 ? Vec2<float>(3 * (keycardSize) + 2 * spacing, 0.55f * (keycardSize)) : Vec2<float>(keycardSize, keycardSize));

                // start glow

                if (bgGlow)
                    FlarialGUI::ShadowRect(
                        positions[i],
                        keycardDims,
                        glowColor, rounde.x,
                        (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

                if (enableGlow && glowAmountModifier[i] > 0.05f)
                    FlarialGUI::ShadowRect(
                        positions[i],
                        keycardDims,
                        glowEnabledColor, rounde.x,
                        (getOps<float>("glowEnabledAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top") * glowAmountModifier[i]);

                // end glow

                if (getOps<bool>("BlurEffect"))
                    FlarialGUI::BlurRect(D2D1::RoundedRect(
                        D2D1::RectF(
                            positions[i].x, positions[i].y,
                            positions[i].x + keycardDims.x, positions[i].y + keycardDims.y),
                        rounde.x, rounde.x));

                if (getOps<bool>("showBg") && getOps<bool>("rectShadow"))
                    FlarialGUI::RoundedRect(
                        positions[i].x + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
                        positions[i].y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
                        getColor("rectShadow"),
                        keycardDims.x,
                        keycardDims.y,
                        rounde.x,
                        rounde.x
                    );

                if (getOps<bool>("border"))
                    FlarialGUI::RoundedHollowRect(
                        positions[i].x,
                        positions[i].y,
                        Constraints::RelativeConstraint((getOps<float>("borderWidth") * getOps<float>("uiscale")) / 100.0f, "height", true),
                        borderColor,
                        keycardDims.x,
                        keycardDims.y,
                        rounde.x,
                        rounde.x
                    );

                if (i == 6) {
                    float spacebarWidth = 3 * (keycardSize) + 2 * spacing;
                    float spacebarHeight = 0.55f * (keycardSize);

                    if (getOps<bool>("showBg")) FlarialGUI::RoundedRect(positions[i].x, positions[i].y, states[i], spacebarWidth, spacebarHeight, rounde.x, rounde.x);

                    float childHeight = Constraints::SpacingConstraint(getOps<float>("spacebarHeight"), spacebarHeight);
                    float childWidth = Constraints::SpacingConstraint(getOps<float>("spacebarWidth"), spacebarWidth);

                    std::pair<float, float> centeredChild = centerChildRectangle(spacebarWidth, spacebarHeight, childWidth, childHeight);

                    if (enableTextShadow)
                        FlarialGUI::RoundedRect(
                            positions[i].x + centeredChild.first + textShadowOffset,
                            positions[i].y + centeredChild.second + textShadowOffset,
                            shadowStates[i], childWidth, childHeight, 0, 0);

                    FlarialGUI::RoundedRect(positions[i].x + centeredChild.first, positions[i].y + centeredChild.second,
                                            textStates[i], childWidth, childHeight, 0, 0);
                } else {
                    if (getOps<bool>("showBg"))
                        FlarialGUI::RoundedRect(positions[i].x, positions[i].y, states[i], keycardDims.x, keycardDims.y,
                                                rounde.x,
                                                rounde.x);

                    if (!hideCPS && !lmbrmb && (i == 4 || i == 5)) {
                        if (enableTextShadow)
                            FlarialGUI::FlarialTextWithFont(
                                positions[i].x + textShadowOffset + textOffset.x,
                                positions[i].y + textShadowOffset + textOffset.y,
                                FlarialGUI::to_wide(i == 4 ? lmbText : rmbText).c_str(), keycardDims.x, keycardDims.y,
                                DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
                                shadowStates[i], true);

                        FlarialGUI::FlarialTextWithFont(positions[i].x + textOffset.x, positions[i].y + textOffset.y,
                                                        FlarialGUI::to_wide(i == 4 ? lmbText : rmbText).c_str(), keycardDims.x, keycardDims.y,
                                                        DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
                                                        textStates[i], true);
                    } else {
                        if (
                            ((i == 4 || i == 5) && lmbrmb) ||
                            (i != 4 && i != 5) ||
                            (((i == 4 || i == 5) && !hideCPS && !lmbrmb))
                        ) {
                            if (enableTextShadow)
                                FlarialGUI::FlarialTextWithFont(
                                    positions[i].x + textShadowOffset + textOffset.x,
                                    positions[i].y + textShadowOffset + textOffset.y,
                                    FlarialGUI::to_wide(opString[i]).c_str(), keycardDims.x, (i != 4 && i != 5) && !hideCPS ? keycardDims.y : Constraints::SpacingConstraint(hideCPS ? 1.0f : 0.65f, keycardSize - (keycardSize * 0.05f)),
                                    DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
                                    shadowStates[i], true);

                            FlarialGUI::FlarialTextWithFont(positions[i].x + textOffset.x, positions[i].y + textOffset.y,
                                                            FlarialGUI::to_wide(opString[i]).c_str(), keycardDims.x, (i != 4 && i != 5) && !hideCPS ? keycardDims.y : Constraints::SpacingConstraint(hideCPS ? 1.0f : 0.65f, keycardSize - (keycardSize * 0.05f)),
                                                            DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL,
                                                            textStates[i], true);
                        }

                        if (!hideCPS && (i == 4 || i == 5)) {
                            if (enableTextShadow)
                                FlarialGUI::FlarialTextWithFont(
                                    positions[i].x + textOffset.x + textShadowOffset * 0.6f,
                                    positions[i].y + textOffset.y + Constraints::SpacingConstraint(0.55, keycardSize - (keycardSize * 0.05f)) + textShadowOffset * 0.6f,
                                    FlarialGUI::to_wide(i == 4 ? lmbText : rmbText).c_str(), keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
                                    Constraints::SpacingConstraint(0.35, keycardSize - (keycardSize * 0.05f)),
                                    DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2,
                                    DWRITE_FONT_WEIGHT_NORMAL, shadowStates[i], true);

                            FlarialGUI::FlarialTextWithFont(positions[i].x + textOffset.x, positions[i].y + textOffset.y +
                                                                                           Constraints::SpacingConstraint(0.55, keycardSize - (keycardSize * 0.05f)),
                                                            FlarialGUI::to_wide(i == 4 ? lmbText : rmbText).c_str(), keycardSize + (keycardSize / 2.0f) + spacing / 2.0f,
                                                            Constraints::SpacingConstraint(0.35, keycardSize - (keycardSize * 0.05f)),
                                                            DWRITE_TEXT_ALIGNMENT_CENTER, fontSize2,
                                                            DWRITE_FONT_WEIGHT_NORMAL, textStates[i], true);
                        }
                    }
                }
            }

            if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
        }
    }
}

void Keystrokes::onRender(RenderEvent &event) {
    if (!this->isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;
    this->normalRender(7, (std::string &) "");
}
