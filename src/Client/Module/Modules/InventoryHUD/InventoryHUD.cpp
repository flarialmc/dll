#include "InventoryHUD.hpp"

#include "Events/EventManager.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "SDK/Client/Render/BaseActorRenderContext.hpp"
#include "Utils/Render/PositionUtils.hpp"

void InventoryHUD::onEnable() {
    Listen(this, RenderEvent, &InventoryHUD::onRender)
    Listen(this, SetupAndRenderEvent, &InventoryHUD::onSetupAndRender)
    if (FlarialGUI::inMenu) {
        FlarialGUI::Notify("To change the position of InventoryHUD, Please click " +
                           ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                               "editmenubind")->value + " in the settings tab.");
    }
    Module::onEnable();
}

void InventoryHUD::onDisable() {
    Deafen(this, RenderEvent, &InventoryHUD::onRender)
    Deafen(this, SetupAndRenderEvent, &InventoryHUD::onSetupAndRender)
    Module::onDisable();
}

void InventoryHUD::defaultConfig() {
    settings.renameSetting("textColor", "textColor_opacity", "textColor_rgb", "text");
    settings.renameSetting("staticDurBarColor", "staticDurBarColor_opacity", "staticDurBarColor_rgb", "staticDurBar");
    settings.renameSetting("specialMaxDurBarColor", "specialMaxDurBarColor_opacity", "specialMaxDurBarColor_rgb", "specialMaxDurBar");

    setDef("textalignment", (std::string)"Right");
    setDef("textscale", 0.05f);
    setDef("textShadow", false);
    setDef("textShadowOffset", 0.003f);
    setDef("showDurBar", true);
    setDef("showStaticDurBarColor", false);
    setDef("overrideSpecialMaxDurBarCol", false);
    setDef("showSpecialMaxDurBarCol", false);
    setDef("uiscale", 1.0f);

    Module::defaultConfig("core");
    Module::defaultConfig("pos");

    setDef("text", (std::string)"FFFFFF", 1.f, false);
    setDef("textShadow", (std::string)"00000", 0.55f, false);
    setDef("staticDurBar", (std::string)"FFFFFF", 1.f, false);
    setDef("specialMaxDurBar", (std::string)"FFFFFF", 1.f, false);
    setDef("spacing", 1.f);
    setDef("textOffsetX", 16.19f);
    setDef("textOffsetY", 13.13f);
    setDef("durBarOffsetX", 1.92f);
    setDef("durBarOffsetY", 12.45f);
    setDef("durBarOpacity", 1.f);
    setDef("100color", 120.f);
    setDef("0color", 0.f);
    setDef("showDurBarMax", false);
    setDef("textOpacity", 1.f);
    
}

void InventoryHUD::settingsRender(float settingsOffset) {

    initSettingsPage();

    addHeader("Inventory HUD");
    addSlider("Size", "", "uiscale", 5.f, 0.f, true);
    addSlider("Text Size", "", "textscale", 0.25f, 0.0f, true);
    addSlider("Spacing", "", "spacing", 10.f, 0.0f, true);
    addToggle("Show Durability", "", "showDurBar");
    addConditionalSlider(getOps<bool>("showDurBar"), "Durability Bar Offset X", "", "durBarOffsetX", 50.f, 0.0f, false);
    addConditionalSlider(getOps<bool>("showDurBar"), "Durability Bar Offset Y", "", "durBarOffsetY", 50.f, 0.0f, false);
    addConditionalSlider(getOps<bool>("showDurBar"), "Durability Bar Opacity", "", "durBarOpacity", 1.f, 0.0f, false);
    addConditionalToggle(getOps<bool>("showDurBar"), "Show Max Durability Bar", "", "showDurBarMax");

    //addSlider("Background Padding", "", "bgPadding", 25.f, 0.0f, false);
    extraPadding();

    addHeader("Text");
    addSlider("Text Offset X", "", "textOffsetX", 50.f, 0.0f, false);
    addSlider("Text Offset Y", "", "textOffsetY", 50.f, 0.0f, false);
    addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, "textalignment", true);
    addToggle("Text Shadow", "Displays a shadow under the text", "textShadow");
    addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Colors");
    addColorPicker("Text Color", "", "text");
    addConditionalColorPicker(getOps<bool>("textShadow"), "Shadow Color", "Text Shadow Color", "textShadow");
    addToggle("Show Special Max Durability Bar Color", "", "showSpecialMaxDurBarCol");
    addConditionalColorPicker(getOps<bool>("showSpecialMaxDurBarCol"), "Special Max Durability Bar Color", "", "specialMaxDurBar");
    addToggle("Enable Static Durability Bar Color", "", "showStaticDurBarColor");
    addConditionalColorPicker(getOps<bool>("showStaticDurBarColor"), "Static Durability Bar Color", "", "staticDurBar");
    addConditionalToggle(getOps<bool>("showStaticDurBarColor"), "Override Special Max Durability Bar Color", "", "overrideSpecialMaxDurBarCol");
    addConditionalSlider(!getOps<bool>("showStaticDurBarColor"), "100% Durability Bar Color", "Hue in degrees", "100color", 360.f, 0.0f, false);
    addConditionalSlider(!getOps<bool>("showStaticDurBarColor"), "0% Durability Bar Color", "Hue in degrees", "0color", 360.f, 0.0f, false);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void InventoryHUD::renderText() {
    float uiscale = getOps<float>("uiscale");
    float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
    float textScale = getOps<float>("textscale") * 1080;

    D2D_COLOR_F textColor = getColor("text");
    D2D1_COLOR_F staticDurBarColor = getColor("staticDurBar");
    D2D1_COLOR_F specialMaxDurBarColor = getColor("specialMaxDurBar");

    float maxDurCol = getOps<float>("100color");
    float minDurCol = getOps<float>("0color");

    float textOpacity = getOps<float>("textOpacity");
    bool textShadow = getOps<bool>("textShadow");
    float ospacing = getOps<float>("spacing");
    float textOffsetX = getOps<float>("textOffsetX");
    float textOffsetY = getOps<float>("textOffsetY");

    bool showSpecialMaxDurBarCol = getOps<bool>("showSpecialMaxDurBarCol");
    bool overrideSpecialMaxDurBarCol = getOps<bool>("overrideSpecialMaxDurBarCol");
    bool showStaticDurBarColor = getOps<bool>("showStaticDurBarColor");

    float durBarOffsetX = getOps<float>("durBarOffsetX");
    float durBarOffsetY = getOps<float>("durBarOffsetY");

    if (this->isEnabled() && ClientInstance::getTopScreenName() == "hud_screen") {
        if (SDK::clientInstance->getLocalPlayer() != nullptr &&
            SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

            float xmodifier = 0.0f;
            float ymodifier = 0.0f;
            int counter = 0;

            //Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

            auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{16, 16}, uiscale);
            float spacing = scaledPos.x * ospacing;

            //float spacing = 16 * uiscale * ospacing;

            for (int i = 9; i < 36; i++) {
                auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(i);
                if (item->getItem() != nullptr) {
                    std::string text = FlarialGUI::cached_to_string(item->count);
                    std::wstring wText(text.begin(), text.end());

                    float textX = currentPos.x + (xmodifier + (textOffsetX * uiscale * guiscale));
                    float textY = currentPos.y + ymodifier + (textOffsetY * uiscale * guiscale);

                    float durPerc = (float)durabilities[i][0] / (float)durabilities[i][1];

                    if (getOps<bool>("showDurBar") && (getOps<bool>("showDurBarMax") || durabilities[i][0] != durabilities[i][1]) && durabilities[i][1] != 0) {
                        D2D1_COLOR_F finalColor = D2D_COLOR_F(D2D1::ColorF::White);

                        if (durabilities[i][0] != 0 || durabilities[i][1] != 0) finalColor = FlarialGUI::HSVtoColorF(minDurCol + durPerc * (maxDurCol - minDurCol), 1.f, 1.f);
                        if (durabilities[i][0] == durabilities[i][1] && durabilities[i][1] != 0 && showSpecialMaxDurBarCol) finalColor = specialMaxDurBarColor;
                        if (showStaticDurBarColor) {
                            if (durabilities[i][0] == durabilities[i][1] && durabilities[i][1] != 0 && overrideSpecialMaxDurBarCol) finalColor = staticDurBarColor;
                            else if (durabilities[i][0] == durabilities[i][1] && !showSpecialMaxDurBarCol) finalColor = staticDurBarColor;
                            else if (durabilities[i][0] != durabilities[i][1]) finalColor = staticDurBarColor;
                        }

                        finalColor.a = getOps<float>("durBarOpacity");

                        FlarialGUI::RoundedRect(
                            currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
                            currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
                            D2D1::ColorF(D2D1::ColorF::Black, getOps<float>("durBarOpacity")),
                            guiscale * uiscale * 13.f,
                            guiscale * uiscale * 2.f,
                            0, 0
                        );
                        FlarialGUI::RoundedRect(
                            currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
                            currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
                            finalColor,
                            durPerc * (guiscale * uiscale * 13.f),
                            guiscale * uiscale * 1.f,
                            0, 0
                        );
                    }

                    if ((float)item->count > 1) {
                        if (getOps<bool>("textShadow"))FlarialGUI::FlarialTextWithFont(
                            textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
                            textY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
                            wText.c_str(),
                            0,
                            0,
                            alignments[getOps<std::string>("textalignment")],
                            textScale * uiscale * guiscale, DWRITE_FONT_WEIGHT_NORMAL,
                            getColor("textShadow"),
                            true
                        );

                        FlarialGUI::FlarialTextWithFont(
                            textX, textY, wText.c_str(),
                            0, 0,
                            alignments[getOps<std::string>("textalignment")],
                            textScale * uiscale * guiscale,
                            DWRITE_FONT_WEIGHT_NORMAL,
                            textColor, true
                        );
                    }
                }

                if (counter < 8) {
                    xmodifier += spacing;
                    counter++;
                }
                else {
                    ymodifier += spacing;
                    xmodifier = 0.0f;
                    counter = 0;
                }
            }
        }
    }
}

void InventoryHUD::onRender(RenderEvent &event) {
    if (ClientInstance::getTopScreenName() == "hud_screen" &&
        this->isEnabled() ||
        ClientInstance::getTopScreenName() == "pause_screen" &&
        this->isEnabled()) {
        float spacing = getOps<float>("spacing");

        Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));

        float uiscale = getOps<float>("uiscale");
        float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();

        float s = 16 * uiscale * guiscale;

        float height = s * 2 * spacing + s;
        float width = s * 8 * spacing + s;

        if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
        else currentPos = Constraints::CenterConstraint(width, height);

        if (ClickGUI::editmenu) {
            FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 40, this->name);
            checkForRightClickAndOpenSettings(currentPos.x, currentPos.y, width, height);
        }

        Vec2<float> vec2;

        vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 40, width, height);

        currentPos.x = vec2.x;
        currentPos.y = vec2.y;

        Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        if (ClickGUI::editmenu) {
            FlarialGUI::UnsetWindowRect();
        }
        renderText();
    }

    if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;
}

void InventoryHUD::onSetupAndRender(SetupAndRenderEvent &event) {
    if (this->isEnabled() && ClientInstance::getTopScreenName() == "hud_screen") {
        auto muirc = event.getMuirc();
        BaseActorRenderContext barc(muirc->getScreenContext(), muirc->getClientInstance(),
                                    muirc->getClientInstance()->getMinecraftGame());

        float uiscale = getOps<float>("uiscale");
        Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

        if (SDK::clientInstance->getLocalPlayer() != nullptr &&
            SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

            float spacing = 16 * uiscale * getOps<float>("spacing");
            float xmodifier = 0.0f;
            float ymodifier = 0.0f;
            int counter = 0;

            for (int i = 9; i < 36; i++) {
                auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(i);
                if (item->getItem() != nullptr) {
                    float maxDamage = item->getMaxDamage();
                    durabilities[i][1] = maxDamage;
                    durabilities[i][0] = maxDamage - item->getDamageValue();
                    // Render the item
                    barc.itemRenderer->renderGuiItemNew(&barc, item, 0,
                                                        scaledPos.x + xmodifier, scaledPos.y + ymodifier,
                                                        1.0f, uiscale, false);

                    if (item->isEnchanted()) {
                        barc.itemRenderer->renderGuiItemNew(&barc, item, 0,
                                                            scaledPos.x + xmodifier, scaledPos.y + ymodifier,
                                                            1.0f, uiscale, true);
                    }
                }

                if (counter < 8) {
                    xmodifier += spacing;
                    counter++;
                }
                else {
                    ymodifier += spacing;
                    xmodifier = 0.0f;
                    counter = 0;
                }
            }
        }
    }
}
