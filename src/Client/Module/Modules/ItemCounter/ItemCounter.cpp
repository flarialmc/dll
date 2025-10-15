#include "ItemCounter.hpp"

#include "Client.hpp"
#include "Events/Game/TickEvent.hpp"
#include "GUI/Engine/Constraints.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Manager.hpp"


void ItemCounter::onEnable() {
    Listen(this, TickEvent, &ItemCounter::onTick)
    Listen(this, RenderEvent, &ItemCounter::onRender)
    Module::onEnable();
}

void ItemCounter::onDisable() {
    Deafen(this, TickEvent, &ItemCounter::onTick)
    Deafen(this, RenderEvent, &ItemCounter::onRender)
    Module::onDisable();
}

void ItemCounter::onSetup() {
    subIndexesToRender.clear();

    for (int i = 1; i < 2000; i++) {
        std::string subIndex = std::format("-{}", i);
        if (this->settings.getSettingByName<std::string>("text" + subIndex) != nullptr) {
            items += 1;
            subIndexesToRender.push_back(i);
        }
    }
}

int ItemCounter::getNextAvailableIndex() {
    int index = 1;
    while (this->settings.getSettingByName<std::string>("itemName-" + FlarialGUI::cached_to_string(index)) != nullptr) {
        index += 1;
        // Safety check to prevent infinite loop
        if (index > 2000) {
            break;
        }
    }
    return index;
}

void ItemCounter::multiDefaultConfig(int subIndexInt) {
    const std::string subIndex = std::format("-{}", subIndexInt);

    setDef("itemName" + subIndex, std::string("item name here"));
    setDef("useCustomSettings" + subIndex, false);
    setDef("showCustomSettings" + subIndex, false);

    setDef("percentageX" + subIndex, 0.0f);
    setDef("percentageY" + subIndex, 0.0f);

    setDef("uiscale" + subIndex, 0.65f);
    setDef("BlurEffect" + subIndex, false);
    setDef("rounding" + subIndex, 32.0f);
    setDef("showBg" + subIndex, true);
    setDef("rectShadow" + subIndex, false);
    setDef("rectShadowOffset" + subIndex, 0.003f);
    setDef("border" + subIndex, false);
    setDef("borderWidth" + subIndex, 1.0f);
    setDef("glow" + subIndex, false);
    setDef("glowAmount" + subIndex, 30.0f);

    setDef("text" + subIndex, (std::string) std::string("{name}: {value}"));
    setDef("textscale" + subIndex, 1.0f);
    setDef("textalignment" + subIndex, (std::string) "Center");
    setDef("textShadow" + subIndex, false);
    setDef("textShadowOffset" + subIndex, 0.003f);

    setDef("text" + subIndex, (std::string) "fafafa", 1.f, false);
    setDef("bg" + subIndex, (std::string) "000000", 0.55f, false);
    setDef("rectShadow" + subIndex, (std::string) "00000", 0.55f, false);
    setDef("textShadow" + subIndex, (std::string) "00000", 0.55f, false);
    setDef("border" + subIndex, (std::string) "000000", 1.f, false);
    setDef("glow" + subIndex, (std::string) "F0F0F0", 1.f, false);

    setDef("responsivewidth" + subIndex, true);
    setDef("reversepaddingx" + subIndex, false);
    setDef("reversepaddingy" + subIndex, false);
    setDef("padx" + subIndex, 0.0f);
    setDef("pady" + subIndex, 0.0f);
    setDef("rectwidth" + subIndex, 1.0f);
    setDef("rectheight" + subIndex, 1.0f);
    setDef("rotation" + subIndex, 0.0f);

}

void ItemCounter::multiClearConfig(int subIndexInt) {
    const std::string subIndex = std::format("-{}", subIndexInt);

    settings.deleteSetting("itemName" + subIndex);
    settings.deleteSetting("useCustomSettings" + subIndex);
    settings.deleteSetting("showCustomSettings" + subIndex);

    settings.deleteSetting("percentageX" + subIndex);
    settings.deleteSetting("percentageY" + subIndex);

    settings.deleteSetting("uiscale" + subIndex);
    settings.deleteSetting("BlurEffect" + subIndex);
    settings.deleteSetting("rounding" + subIndex);
    settings.deleteSetting("showBg" + subIndex);
    settings.deleteSetting("rectShadow" + subIndex);
    settings.deleteSetting("rectShadowOffset" + subIndex);
    settings.deleteSetting("border" + subIndex);
    settings.deleteSetting("borderWidth" + subIndex);
    settings.deleteSetting("glow" + subIndex);
    settings.deleteSetting("glowAmount" + subIndex);

    settings.deleteSetting("text" + subIndex);
    settings.deleteSetting("textscale" + subIndex);
    settings.deleteSetting("textalignment" + subIndex);
    settings.deleteSetting("textShadow" + subIndex);
    settings.deleteSetting("textShadowOffset" + subIndex);

    for (std::string colSetting: std::vector<std::string>{ "text", "bg", "rectShadow",  "textShadow", "border", "glow"}) {
        settings.deleteSetting(colSetting + subIndex + "Col");
        settings.deleteSetting(colSetting + subIndex + "RGB");
        settings.deleteSetting(colSetting + subIndex + "Opacity");
    }

    settings.deleteSetting("responsivewidth" + subIndex);
    settings.deleteSetting("reversepaddingx" + subIndex);
    settings.deleteSetting("reversepaddingy" + subIndex);
    settings.deleteSetting("padx" + subIndex);
    settings.deleteSetting("pady" + subIndex);
    settings.deleteSetting("rectwidth" + subIndex);
    settings.deleteSetting("rectheight" + subIndex);
    settings.deleteSetting("rotation" + subIndex);

}

void ItemCounter::defaultConfig() {
    Module::defaultConfig("all");
}

void ItemCounter::multiSettingsRender(int subIndexInt) {

    std::string subIndex = std::format("-{}", subIndexInt);

    bool c = getOps<bool>("useCustomSettings" + subIndex) && getOps<bool>("showCustomSettings" + subIndex);

    if (c) addHeader(std::format("Main {}", subIndexInt));
    addConditionalSlider(c, "UI Scale", "", settings.getSettingByName<float>("uiscale" + subIndex)->value, 2.0f);
    addConditionalToggle(c, "Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", settings.getSettingByName<bool>("BlurEffect" + subIndex)->value);
    addConditionalSlider(c, "Rounding", "Rounding of the rectangle", settings.getSettingByName<float>("rounding" + subIndex)->value);
    addConditionalToggle(c, "Background", "", settings.getSettingByName<bool>("showBg" + subIndex)->value);
    addConditionalToggle(c && getOps<bool>("showBg" + subIndex), "Background Shadow", "Displays a shadow under the background", settings.getSettingByName<bool>("rectShadow" + subIndex)->value);
    addConditionalSlider(c && getOps<bool>("showBg" + subIndex) && getOps<bool>("rectShadow" + subIndex), "Shadow Offset", "How far the shadow will be.", settings.getSettingByName<float>("rectShadowOffset" + subIndex)->value, 0.02f, 0.001f);
    addConditionalToggle(c, "Border", "", settings.getSettingByName<bool>("border" + subIndex)->value);
    addConditionalSlider(c && getOps<bool>("border" + subIndex), "Border Thickness", "", settings.getSettingByName<float>("borderWidth" + subIndex)->value, 4.f);
    addConditionalToggle(c, "Glow", "", settings.getSettingByName<bool>("glow" + subIndex)->value);
    addConditionalSlider(c && getOps<bool>("glow" + subIndex), "Glow Amount", "", settings.getSettingByName<float>("glowAmount" + subIndex)->value, 100.f);
    if (c) extraPadding();

    if (c) addHeader(std::format("Text {}", subIndexInt));
    addConditionalTextBox(c, "Format", "Use {value} for the value and {name} for the name.", 50, "text" + subIndex);
    addConditionalSlider(c, "Text Scale", "", settings.getSettingByName<float>("textscale" + subIndex)->value, 2.0f);
    addConditionalDropdown(c, "Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, settings.getSettingByName<std::string>("textalignment" + subIndex)->value, true);
    addConditionalToggle(c, "Text Shadow", "Displays a shadow under the text", settings.getSettingByName<bool>("textShadow" + subIndex)->value);
    addConditionalSlider(getOps<bool>("textShadow" + subIndex), "Shadow Offset", "How far the shadow will be.", settings.getSettingByName<float>("textShadowOffset" + subIndex)->value, 0.02f, 0.001f);
    if (c) extraPadding();

    if (c) addHeader(std::format("Colors {}", subIndexInt));
    addConditionalColorPicker(
        c, "Text Color", "",
        settings.getSettingByName<std::string>("text" + subIndex + "Col")->value,
        settings.getSettingByName<float>("text" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("text" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("showBg" + subIndex), "Background Color", "",
        settings.getSettingByName<std::string>("bg" + subIndex + "Col")->value,
        settings.getSettingByName<float>("bg" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("bg" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("rectShadow" + subIndex), "Background Shadow Color", "",
        settings.getSettingByName<std::string>("rectShadow" + subIndex + "Col")->value,
        settings.getSettingByName<float>("rectShadow" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("rectShadow" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("textShadow" + subIndex), "Text Shadow Color", "",
        settings.getSettingByName<std::string>("textShadow" + subIndex + "Col")->value,
        settings.getSettingByName<float>("textShadow" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("textShadow" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("border" + subIndex), "Border Color", "",
        settings.getSettingByName<std::string>("border" + subIndex + "Col")->value,
        settings.getSettingByName<float>("border" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("border" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("glow" + subIndex), "Glow Color", "",
        settings.getSettingByName<std::string>("glow" + subIndex + "Col")->value,
        settings.getSettingByName<float>("glow" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("glow" + subIndex + "RGB")->value
    );
    if (c) extraPadding();

    if (c) addHeader(std::format("Misc {}", subIndexInt));
    addConditionalToggle(c, "Responsive Rectangle", "Rectangle resizes with text", settings.getSettingByName<bool>("responsivewidth" + subIndex)->value);
    addConditionalToggle(c, "Reverse Padding X", "For Text Position", settings.getSettingByName<bool>("reversepaddingx" + subIndex)->value);
    addConditionalToggle(c, "Reverse Padding Y", "For Text Position", settings.getSettingByName<bool>("reversepaddingy" + subIndex)->value);
    addConditionalSlider(c, "Padding X", "For Text Position", settings.getSettingByName<float>("padx" + subIndex)->value, 1.f);
    addConditionalSlider(c, "Padding Y", "For Text Position", settings.getSettingByName<float>("pady" + subIndex)->value, 1.f);
    addConditionalSlider(c, "Rectangle Width", "", settings.getSettingByName<float>("rectwidth" + subIndex)->value, 2.f, 0.001f);
    addConditionalSlider(c, "Rectangle Height", "", settings.getSettingByName<float>("rectheight" + subIndex)->value, 2.f, 0.001f);
    addConditionalSlider(c, "Rotation", "", settings.getSettingByName<float>("rotation" + subIndex)->value, 360.f, 0, false);

}

void ItemCounter::settingsRender(float settingsOffset) {
    initSettingsPage();

    addButton("Add Item", "adds an item", "Add", [this] {
        int index = getNextAvailableIndex();
        multiDefaultConfig(index);
        Client::SaveSettings();
        FlarialGUI::Notify(std::format("Successfully created new item counter with index <{}>!", index));
        subIndexesToRender.push_back(index);
        items += 1;
    });

    extraPadding();

    std::vector<int> subIndexesToRenderCopy = subIndexesToRender;
    for (auto i: subIndexesToRenderCopy) {
        std::string subIndex = std::format("-{}", i);

        addHeader(std::format("Item - {}", i));

        addTextBox("Item Name", "item name", settings.getSettingByName<std::string>("itemName" + subIndex)->value, 50);
        addToggle("Use Custom Settings", "allows you to change render settings specifically for this item", settings.getSettingByName<bool>("useCustomSettings" + subIndex)->value);
        addConditionalToggle(getOps<bool>("useCustomSettings" + subIndex), "Show Custom Settings", "", settings.getSettingByName<bool>("showCustomSettings" + subIndex)->value);

        multiSettingsRender(i);

        addButton("Delete item", "deletes the item", "Delete", [this, i] {
            auto it = std::ranges::find(subIndexesToRender, i);
            if (it != subIndexesToRender.end()) subIndexesToRender.erase(it);
            multiClearConfig(i);
            Client::SaveSettings();
            FlarialGUI::Notify(std::format("Successfully removed item counter with index <{}>!", i));
            items--;
        });

        extraPadding();
    }

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ItemCounter::multiNormalRenderCore(int index, std::string &text, int subIndexInt, const bool useCustom) {
    if (!this->isEnabled()) return;

    std::string subIndex = std::format("-{}", subIndexInt);
    std::string subIndex2 = subIndex;
    if (!useCustom) subIndex = "";

    float rotation = getOps<float>("rotation" + subIndex);
    DWRITE_TEXT_ALIGNMENT alignment = alignments[getOps<std::string>("textalignment" + subIndex)];
    bool responsivewidth = getOps<bool>("responsivewidth" + subIndex);
    float paddingX = getOps<float>("padx" + subIndex);
    float paddingY = getOps<float>("pady" + subIndex);

    if (getOps<bool>("reversepaddingx" + subIndex)) paddingX = -(getOps<float>("padx" + subIndex));
    if (getOps<bool>("reversepaddingy" + subIndex)) paddingY = -(getOps<float>("pady" + subIndex));

    float textWidth = Constraints::RelativeConstraint(0.7f * getOps<float>("uiscale" + subIndex));
    float textHeight = Constraints::RelativeConstraint(0.1f * getOps<float>("uiscale" + subIndex));
    float textSize = Constraints::SpacingConstraint(3.2f, textHeight) * getOps<float>("textscale" + subIndex);

    Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX" + subIndex2), getOps<float>("percentageY" + subIndex2));

    float realspacing = Constraints::SpacingConstraint(0.05f, textWidth);

    ImVec2 textMetrics = FlarialGUI::getFlarialTextSize(
        FlarialGUI::to_wide(text).c_str(),
        1000000,
        textHeight,
        alignment,
        textSize, DWRITE_FONT_WEIGHT_NORMAL, true
    );

    float rectWidth = (
        !responsivewidth
            ? (Constraints::RelativeConstraint(0.225f * getOps<float>("uiscale" + subIndex)) * getOps<float>("rectwidth" + subIndex))
            : (textMetrics.x + Constraints::SpacingConstraint(2.0, realspacing)) * getOps<float>("rectwidth" + subIndex)
    );
    float rectHeight = textHeight * getOps<float>("rectheight" + subIndex);

    Vec2<float> topleft;

    if (settingperc.x != 0) topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    else topleft = Constraints::CenterConstraint(rectWidth, rectHeight);

    if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_JUSTIFIED) prevAlignments[index] = alignment;

    if (prevAlignments[index] != alignment) {
        float toAdjust = 0;
        auto prev = prevAlignments[index];

        if (prev != alignment) {
            if (prev == DWRITE_TEXT_ALIGNMENT_CENTER) {
                toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_LEADING ? -0.5f : 0.5f) * rectWidth;
            } else if (prev == DWRITE_TEXT_ALIGNMENT_LEADING) {
                toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_CENTER ? 0.5f : 1.f) * rectWidth;
            } else if (prev == DWRITE_TEXT_ALIGNMENT_TRAILING) {
                toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_CENTER ? -0.5f : -1.f) * rectWidth;
            }
        }

        settings.setValue("percentageX" + subIndex2, (topleft.x + toAdjust) / MC::windowSize.x);

        settingperc = Vec2<float>(getOps<float>("percentageX" + subIndex2), getOps<float>("percentageY" + subIndex2));
        topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    }

    prevAlignments[index] = alignment;

    if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
        topleft.x -= (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);
    }

    if (ClickGUI::editmenu) {
        FlarialGUI::SetWindowRect(topleft.x, topleft.y, rectWidth, rectHeight, index, this->name + subIndex);

        checkForRightClickAndOpenSettings(topleft.x, topleft.y, rectWidth, rectHeight);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(topleft.x, topleft.y, index, rectWidth, rectHeight);

        // if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
        //     if (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING) vec2.x += rectWidth;
        //     else vec2.x += rectWidth / 2.f;
        // }

        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) vec2.x += (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);

        topleft.x = vec2.x;
        topleft.y = vec2.y;

        Vec2<float> percentages = Constraints::CalculatePercentage(topleft.x, topleft.y, 0, 0);
        settings.setValue("percentageX" + subIndex2, percentages.x);
        settings.setValue("percentageY" + subIndex2, percentages.y);

        // if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
        //     if (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING) topleft.x -= rectWidth;
        //     else topleft.x -= rectWidth / 2.f;
        // }
        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) topleft.x -= (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);
    }

    Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding" + subIndex) * getOps<float>("uiscale" + subIndex), getOps<float>("rounding" + subIndex) * getOps<float>("uiscale" + subIndex));
    ImVec2 rotationCenter;

    if (rotation > 0.0f) {
        rotationCenter = ImVec2(topleft.x + rectWidth / 2.0f, topleft.y + rectHeight / 2.0f);
        FlarialGUI::ImRotateStart();
    }

    if (getOps<bool>("glow" + subIndex))
        FlarialGUI::ShadowRect(
            Vec2<float>(topleft.x, topleft.y),
            Vec2<float>(rectWidth, textHeight * getOps<float>("rectheight" + subIndex)),
            getColor("glow" + subIndex), rounde.x,
            (getOps<float>("glowAmount" + subIndex) / 100.f) * Constraints::PercentageConstraint(0.1f, "top" + subIndex));

    float blur = Client::settings.getSettingByName<float>("blurintensity")->value;
    if (getOps<bool>("BlurEffect" + subIndex)) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(topleft.x, topleft.y, topleft.x + rectWidth, topleft.y + rectHeight), rounde.x, rounde.x));
    //Blur::RenderBlur(SwapchainHook::mainRenderTargetView, 3, blur, topleft.x, topleft.y, rectWidth, rectHeight, rounde.x);


    if (getOps<bool>("rectShadow" + subIndex))
        FlarialGUI::RoundedRect(
            topleft.x + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset" + subIndex)) * getOps<float>("uiscale" + subIndex),
            topleft.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset" + subIndex)) * getOps<float>("uiscale" + subIndex),
            getColor("rectShadow" + subIndex),
            rectWidth,
            rectHeight,
            rounde.x,
            rounde.x
        );

    if (getOps<bool>("showBg" + subIndex))
        FlarialGUI::RoundedRect(
            topleft.x,
            topleft.y,
            getColor("bg" + subIndex),
            rectWidth,
            rectHeight,
            rounde.x,
            rounde.x
        );

    if (getOps<bool>("textShadow" + subIndex))
        FlarialGUI::FlarialTextWithFont(
            topleft.x + Constraints::SpacingConstraint(paddingX, textWidth) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset" + subIndex)) * getOps<float>("uiscale" + subIndex),
            topleft.y + Constraints::SpacingConstraint(paddingY, textWidth) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset" + subIndex)) * getOps<float>("uiscale" + subIndex),
            FlarialGUI::to_wide(text).c_str(),
            rectWidth,
            rectHeight,
            alignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            getColor("textShadow" + subIndex),
            true
        );

    if (false) {
        auto txtCol = getColor("text" + subIndex);
        float guiScale = SDK::clientInstance->getGuiData()->getGuiScale();

        SDK::pushDrawTextQueueEntry(DrawTextQueueEntry(
            text,
            RectangleArea(
                (topleft.x + Constraints::SpacingConstraint(paddingX, textWidth)) / guiScale,
                0,
                (topleft.y + Constraints::SpacingConstraint(paddingY, textWidth)) / guiScale,
                0),
            MCCColor(txtCol.r, txtCol.g, txtCol.b, txtCol.a),
            static_cast<ui::TextAlignment>(alignment),
            TextMeasureData(getOps<float>("textscale" + subIndex) * 2.f, getOps<bool>("textShadow" + subIndex), false),
            CaretMeasureData{-1, 0}
        ));
    } else {
        FlarialGUI::FlarialTextWithFont(
            topleft.x + Constraints::SpacingConstraint(paddingX, textWidth),
            topleft.y + Constraints::SpacingConstraint(paddingY, textWidth),
            FlarialGUI::to_wide(text).c_str(),
            rectWidth,
            rectHeight,
            alignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            getColor("text" + subIndex),
            true
        );
    }


    if (getOps<bool>("border" + subIndex))
        FlarialGUI::RoundedHollowRect(
            topleft.x,
            topleft.y,
            Constraints::RelativeConstraint((getOps<float>("borderWidth" + subIndex) * getOps<float>("uiscale" + subIndex)) / 100.0f, "height" + subIndex, true),
            getColor("border" + subIndex),
            rectWidth,
            textHeight * getOps<float>("rectheight" + subIndex),
            rounde.x,
            rounde.x
        );

    if (rotation > 0.0f) FlarialGUI::ImRotateEnd(rotation + 90.f, rotationCenter);

    if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
}

void ItemCounter::onTick(TickEvent& event) {
    if (!this->isEnabled()) return;
    if (!SDK::hasInstanced || SDK::clientInstance == nullptr) return;

    if (SDK::getCurrentScreen() != "hud_screen") return;

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player || !player->getSupplies()) return;

    auto supplies = player->getSupplies();
    auto inventory = supplies->getInventory();
    if (!inventory) return;


    for (int i: subIndexesToRender) {
        int itemCount = 0;
        ItemStack* offhandItem = player->getOffhandSlot();

        SettingType<std::string>* s = settings.getSettingByName<std::string>(std::format("itemName-{}", i));
        if (s == nullptr) continue;

        std::string itemName = String::toLower(s->value);

        if (itemName.empty() || itemName == "item name here") continue;

        std::ranges::replace(itemName, ' ', '_');

        if (offhandItem && offhandItem->getItem() && String::toLower(offhandItem->getItem()->name) == itemName) {
            itemCount = offhandItem->count;
        }

        for (int i = 0; i < 36; i++) {
            auto item = inventory->getItem(i);

            if (item->getItem() != nullptr) {
                if (String::toLower(item->getItem()->name) == itemName) {
                    itemCount += item->count;
                }

            }
        }

        itemCountMap[s->value] = itemCount;
    }
}

void ItemCounter::onRender(RenderEvent& event) {
    if (!this->isEnabled()) return;
    if (SDK::clientInstance->getScreenName() != "hud_screen") return;
    std::shared_ptr<Module> cgui = ModuleManager::getModule("ClickGUI");
    if (!cgui || cgui->active) return;

    int _temp = 71;
    for (int i: subIndexesToRender) {

        std::string text{};
        if (this->settings.getSettingByName<std::string>(std::format("text-{}", i)) != nullptr) text = getOps<std::string>(std::format("text-{}", i));

        std::string uppercaseSentence;
        for (char c: text) {
            uppercaseSentence += (char) std::toupper(c);
        }

        std::string searchValue = "{VALUE}";

        size_t posValue = uppercaseSentence.find(searchValue);
        if (posValue != std::string::npos) {
            text.replace(posValue, searchValue.length(), std::to_string(itemCountMap[getOps<std::string>(std::format("itemName-{}", i))]));
        }

        for (char c: text) {
            uppercaseSentence += (char) std::toupper(c);
        }

        std::string searchName = "{NAME}";

        size_t posName = uppercaseSentence.find(searchName);
        if (posName != std::string::npos) {
            text.replace(posName, searchName.length(), text);
        }

        multiNormalRenderCore(_temp, text, i, getOps<bool>(std::format("useCustomSettings-{}", i)));
        _temp += 1;
    }

}
