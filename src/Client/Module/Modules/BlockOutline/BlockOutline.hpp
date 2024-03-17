#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../../Utils/Memory/CustomAllocator/Buffer.hpp" // why its not hpp here ???

class BlockOutline : public Module {

private:
    const static inline auto highlightColorRipRelAddr = Memory::findSig("? ? ? ? 0F 11 45 ? 0F 11 00 C6 40 10 ? 45 38 96"); // RIP REL 4BYTE FROM FUNC OFFSET ADDR
    static inline auto highlightColorOrigRipRel = *(UINT32*)highlightColorRipRelAddr;

    const static inline auto outlineColorRipRelAddr = Memory::findSig("? ? ? ? 0F 11 00 C6 40 10 ? 0F 57 C9 0F 11 4D");
    static inline auto outlineColorOrigRipRel = *(UINT32*)outlineColorRipRelAddr;
    // TODO: make it look better
    static inline auto selectionColor = (MCCColor*)AllocateBuffer((void*)highlightColorRipRelAddr); // allocate space for color
    static inline auto highlightColorNewRipRel = Memory::getRipRel(highlightColorRipRelAddr, reinterpret_cast<uintptr_t>((void*)selectionColor));
    static inline auto outlineColorNewRipRel= Memory::getRipRel(outlineColorRipRelAddr, reinterpret_cast<uintptr_t>((void*)selectionColor));

public:
    BlockOutline() : Module("BlockOutline", "Changes the block outline color", R"(\Flarial\assets\block.png)", "") {
        Module::setup();
    };

    void onSetup() override { // init color just in case
        *selectionColor = MCCColor{};
    }

    void terminate() override { // free memory we took for color
        FreeBuffer(selectionColor);
        Module::terminate();
    }

    void onColorChange() {
        // change our color
        D2D1_COLOR_F color;
        if (settings.getSettingByName<bool>("color_rgb")->value) color = FlarialGUI::rgbColor;
        else color = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("color")->value);

        selectionColor->r = color.r;
        selectionColor->g = color.g;
        selectionColor->b = color.b;
        selectionColor->a = settings.getSettingByName<float>("colorOpacity")->value;
    }

    static void patch() { // change rel rip offset to ours
        Memory::patchBytes((void *) highlightColorRipRelAddr, highlightColorNewRipRel.data(), sizeof(UINT32));
        Memory::patchBytes((void *) outlineColorRipRelAddr, outlineColorNewRipRel.data(), sizeof(UINT32));
    }

    static void unpatch() { // change rel rip offset to ours
        Memory::patchBytes((void *) highlightColorRipRelAddr, &highlightColorOrigRipRel, sizeof(UINT32));
        Memory::patchBytes((void *) outlineColorRipRelAddr, &outlineColorOrigRipRel, sizeof(UINT32));
    }

    void onEnable() override {
        onColorChange();
        patch();
        Module::onEnable();
    }

    void onDisable() override {
        unpatch();
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.6f);

    }

    void settingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(x, y, L"Color", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(0, x + Constraints::SpacingConstraint(0.95, textWidth),
                                y - Constraints::SpacingConstraint(0.017, textWidth),
                                settings.getSettingByName<std::string>("color")->value,
                                settings.getSettingByName<float>("colorOpacity")->value,
                                settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);
        if (settings.getSettingByName<bool>("enabled"))
            onColorChange();
    }

};
