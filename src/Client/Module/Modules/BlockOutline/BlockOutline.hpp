#pragma once

#include <Modules/Module.hpp>
#include <Utils/Utils.hpp>
#include <Utils/Memory/CustomAllocator/Buffer.hpp>

#include <Utils/Render/DrawUtil3D.hpp>

class BlockOutline : public Module {
// TODO: switch to renderOutlineSelection hook + 3D option
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
    BlockOutline() : Module("Block Outline", "Changes the block outline color", IDR_BLOCK_PNG, "") {
        Module::setup();
    };

    void onSetup() override {
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
        Listen(this, RenderOutlineSelectionEvent, &BlockOutline::onOutlineSelection)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderOutlineSelectionEvent, &BlockOutline::onOutlineSelection)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.6f);

    }

    void settingsRender(float settingsOffset) override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Colors");
        this->addColorPicker("Outline Color", "", settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();


        if (settings.getSettingByName<bool>("enabled"))
            onColorChange();
    }

    void onOutlineSelection(RenderOutlineSelectionEvent &event) {
        MCDrawUtil3D dc(SDK::clientInstance->getLevelRender(), event.getScreenContext());
        D2D1_COLOR_F color;
        if (settings.getSettingByName<bool>("color_rgb")->value) color = FlarialGUI::rgbColor;
        else color = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("color")->value);

        selectionColor->r = color.r;
        selectionColor->g = color.g;
        selectionColor->b = color.b;
        selectionColor->a = settings.getSettingByName<float>("colorOpacity")->value;
        dc.drawBox(event.getBox(), color);
        dc.flush();

        event.cancel();
    }
};
