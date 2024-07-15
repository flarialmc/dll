#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../../Utils/Memory/CustomAllocator/Buffer.hpp" // why its not hpp here ???

class BlockOutline : public Module {

private:
    static inline uintptr_t highlightColorRipRelAddr; // RIP REL 4BYTE FROM FUNC OFFSET ADDR
    static inline UINT32 highlightColorOrigRipRel;

    static inline uintptr_t outlineColorRipRelAddr;
    static inline UINT32 outlineColorOrigRipRel;
    // TODO: make it look better
    static inline MCCColor* selectionColor; // allocate space for color
    static inline std::array<std::byte, 4> highlightColorNewRipRel;
    static inline std::array<std::byte, 4> outlineColorNewRipRel;

public:
    BlockOutline() : Module("BlockOutline", "Changes the block outline color", IDR_BLOCK_PNG, "") {
        Module::setup();
    };

    void onSetup() override { // init color just in case
        highlightColorRipRelAddr = Memory::findSig(GET_SIG("blockHighlightColor")); // RIP REL 4BYTE FROM FUNC OFFSET ADDR
        if(highlightColorRipRelAddr == NULL) return;
        highlightColorOrigRipRel = *(UINT32*)highlightColorRipRelAddr;

        outlineColorRipRelAddr = Memory::findSig(GET_SIG("mce::Color::BLACK"));
        if(outlineColorRipRelAddr == NULL) return;
        outlineColorOrigRipRel = *(UINT32*)outlineColorRipRelAddr;
        // TODO: make it look better
        selectionColor = (MCCColor*)AllocateBuffer((void*)highlightColorRipRelAddr); // allocate space for color
        highlightColorNewRipRel = Memory::getRipRel(highlightColorRipRelAddr, reinterpret_cast<uintptr_t>((void*)selectionColor));
        outlineColorNewRipRel= Memory::getRipRel(outlineColorRipRelAddr, reinterpret_cast<uintptr_t>((void*)selectionColor));
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
        if(highlightColorRipRelAddr == NULL || outlineColorRipRelAddr == NULL) return;
        Memory::patchBytes((void *) highlightColorRipRelAddr, highlightColorNewRipRel.data(), sizeof(UINT32));
        Memory::patchBytes((void *) outlineColorRipRelAddr, outlineColorNewRipRel.data(), sizeof(UINT32));
    }

    static void unpatch() { // change rel rip offset to ours
        if(highlightColorRipRelAddr == NULL || outlineColorRipRelAddr == NULL) return;
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
                                settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);
        if (settings.getSettingByName<bool>("enabled"))
            onColorChange();
    }

};
