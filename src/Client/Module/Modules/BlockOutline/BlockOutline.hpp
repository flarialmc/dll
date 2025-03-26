#pragma once

#include <Modules/Module.hpp>
#include <Utils/Utils.hpp>
#include <Utils/Memory/CustomAllocator/Buffer.hpp>

#include <Utils/Render/DrawUtil3D.hpp>

class BlockOutline : public Module {

public:
    BlockOutline() : Module("Block Outline", "Changes the block outline color", IDR_BLOCK_PNG, "") {
        Module::setup();
    };

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
    }

    void onOutlineSelection(RenderOutlineSelectionEvent &event) {
        MCDrawUtil3D dc(SDK::clientInstance->getLevelRender(), event.getScreenContext());
        D2D1_COLOR_F color;
        color = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("color")->value);
        color.a = settings.getSettingByName<float>("colorOpacity")->value;
        
        auto bp = event.getPos();

        auto drawUp = [&]() {
            dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + 1.f), Vec3<float>(bp.x, bp.y + 1.f, bp.z + 1.f ), color);
            };

        auto drawDown = [&]() {
            dc.fillQuad(bp, Vec3<float>(bp.x + 1.f, bp.y, bp.z), Vec3<float>(bp.x + 1.f, bp.y, bp.z + 1.f), Vec3<float>(bp.x, bp.y, bp.z + 1.f), color);
            };

        auto drawEast = [&]() {
            dc.fillQuad(Vec3<float>(bp.x + 1.f, bp.y, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + 1.f), Vec3<float>(bp.x + 1.f, bp.y, bp.z + 1.f), color);
            };

        auto drawWest = [&]() {
            dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z), Vec3<float>(bp.x, bp.y + 1.f, bp.z), Vec3<float>(bp.x, bp.y + 1.f, bp.z + 1.f), Vec3<float>(bp.x, bp.y, bp.z + 1.f), color);
            };

        auto drawSouth = [&]() {
            dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z + 1.f), Vec3<float>(bp.x, bp.y + 1.f, bp.z + 1.f), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + 1.f), Vec3<float>(bp.x + 1.f, bp.y, bp.z + 1.f), color);
            };

        auto drawNorth = [&]() {
            dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z), Vec3<float>(bp.x, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y, bp.z), color);
            };

        drawNorth();
        drawEast();
        drawSouth();
        drawWest();
        drawUp();
        drawDown();

        dc.flush();

        event.cancel();
    }
};
