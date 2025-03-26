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
        if (settings.getSettingByName<bool>("overlay") == nullptr) settings.addSetting("overlay", false);
        if (settings.getSettingByName<bool>("overlayfullblock") == nullptr) settings.addSetting("overlayfullblock", false);
        if (settings.getSettingByName<float>("outlinewidth") == nullptr) settings.addSetting("outlinewidth", 0.01f);

    }

    void settingsRender(float settingsOffset) override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Main");
        this->addToggle("Overlay", "Overlays the face of block", settings.getSettingByName<bool>("overlay")->value);
        std::string col = settings.getSettingByName<bool>("overlay")->value ? "Overlay" : "Outline";
        if (settings.getSettingByName<bool>("overlay")->value) {
            this->addToggle("Overlay Full Block", "Overlays the full block", settings.getSettingByName<bool>("overlayfullblock")->value);
        }
        else {
            this->addSlider("Outline Width", "Thickness of the outline", settings.getSettingByName<float>("outlinewidth")->value, 0.5);
        }
        this->addColorPicker(col + " Color", "", settings.getSettingByName<std::string>("color")->value,
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

        auto face = (int)SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult().facing;

        auto bp = event.getPos();

        if (!settings.getSettingByName<bool>("overlay")->value) {

            auto drawUp = [&](float width) {
                float w = width;
                float wi = 1 - w;
                //Top Left 
                dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + w), Vec3<float>(bp.x, bp.y + 1.f, bp.z + w), color);

                //Top Right
                dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.f, bp.z + wi), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + wi), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + 1), Vec3<float>(bp.x, bp.y + 1.f, bp.z + 1), color);

                //Top Top
                dc.fillQuad(Vec3<float>(bp.x + wi, bp.y + 1.f, bp.z + w), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + w), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + 1 - w), Vec3<float>(bp.x + wi, bp.y + 1.f, bp.z + 1), color);

                //Top Bottom
                dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.f, bp.z + w), Vec3<float>(bp.x + w, bp.y + 1.f, bp.z + w), Vec3<float>(bp.x + w, bp.y + 1.f, bp.z + 1 - w), Vec3<float>(bp.x, bp.y + 1.f, bp.z + 1 - w), color);
                };

            auto drawDown = [&](float width) {
                float w = width;
                float wi = 1 - w;
                //Bottom Left 
                dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z), Vec3<float>(bp.x + 1.f, bp.y, bp.z), Vec3<float>(bp.x + 1.f, bp.y, bp.z + w), Vec3<float>(bp.x, bp.y, bp.z + w), color);

                //Bottom Right
                dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z + wi), Vec3<float>(bp.x + 1.f, bp.y, bp.z + wi), Vec3<float>(bp.x + 1.f, bp.y, bp.z + 1), Vec3<float>(bp.x, bp.y, bp.z + 1), color);

                //Bottom Top
                dc.fillQuad(Vec3<float>(bp.x + wi, bp.y, bp.z + w), Vec3<float>(bp.x + 1.f, bp.y, bp.z + w), Vec3<float>(bp.x + 1.f, bp.y, bp.z + 1 - w), Vec3<float>(bp.x + wi, bp.y, bp.z + 1), color);

                //Bottom Bottom
                dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z + w), Vec3<float>(bp.x + w, bp.y, bp.z + w), Vec3<float>(bp.x + w, bp.y, bp.z + 1 - w), Vec3<float>(bp.x, bp.y, bp.z + 1 - w), color);

                };
            auto drawEast = [&](float width) {
                float w = width;
                float wi = 1 - w;
                // Bottom strip (along y = bp.y)
                dc.fillQuad(Vec3<float>(bp.x + 1.f, bp.y, bp.z),
                    Vec3<float>(bp.x + 1.f, bp.y + w, bp.z),
                    Vec3<float>(bp.x + 1.f, bp.y + w, bp.z + 1.f),
                    Vec3<float>(bp.x + 1.f, bp.y, bp.z + 1.f), color);
                // Top strip (along y = bp.y + 1.f)
                dc.fillQuad(Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z),
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z),
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + 1.f),
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z + 1.f), color);
                // Front strip (along z = bp.z)
                dc.fillQuad(Vec3<float>(bp.x + 1.f, bp.y + w, bp.z),
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z),
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z + w),
                    Vec3<float>(bp.x + 1.f, bp.y + w, bp.z + w), color);
                // Back strip (along z = bp.z + 1.f)
                dc.fillQuad(Vec3<float>(bp.x + 1.f, bp.y + w, bp.z + 1.f - w),
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z + 1.f - w),
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z + 1.f),
                    Vec3<float>(bp.x + 1.f, bp.y + w, bp.z + 1.f), color);
                };

            auto drawWest = [&](float width) {
                float w = width;
                float wi = 1 - w;
                // Bottom strip (along y = bp.y)
                dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z),
                    Vec3<float>(bp.x, bp.y + w, bp.z),
                    Vec3<float>(bp.x, bp.y + w, bp.z + 1.f),
                    Vec3<float>(bp.x, bp.y, bp.z + 1.f), color);
                // Top strip (along y = bp.y + 1.f)
                dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.f - w, bp.z),
                    Vec3<float>(bp.x, bp.y + 1.f, bp.z),
                    Vec3<float>(bp.x, bp.y + 1.f, bp.z + 1.f),
                    Vec3<float>(bp.x, bp.y + 1.f - w, bp.z + 1.f), color);
                // Front strip (along z = bp.z)
                dc.fillQuad(Vec3<float>(bp.x, bp.y + w, bp.z),
                    Vec3<float>(bp.x, bp.y + 1.f - w, bp.z),
                    Vec3<float>(bp.x, bp.y + 1.f - w, bp.z + w),
                    Vec3<float>(bp.x, bp.y + w, bp.z + w), color);
                // Back strip (along z = bp.z + 1.f)
                dc.fillQuad(Vec3<float>(bp.x, bp.y + w, bp.z + 1.f - w),
                    Vec3<float>(bp.x, bp.y + 1.f - w, bp.z + 1.f - w),
                    Vec3<float>(bp.x, bp.y + 1.f - w, bp.z + 1.f),
                    Vec3<float>(bp.x, bp.y + w, bp.z + 1.f), color);
                };

            auto drawSouth = [&](float width) {
                float w = width;
                // Bottom strip (along y = bp.y, z = bp.z + 1.f)
                dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z + 1.f),           // Bottom-left
                    Vec3<float>(bp.x + 1.f, bp.y, bp.z + 1.f),     // Bottom-right
                    Vec3<float>(bp.x + 1.f, bp.y + w, bp.z + 1.f), // Top-right
                    Vec3<float>(bp.x, bp.y + w, bp.z + 1.f),       // Top-left
                    color);
                // Top strip (along y = bp.y + 1.f, z = bp.z + 1.f)
                dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.f - w, bp.z + 1.f),     // Bottom-left
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z + 1.f), // Bottom-right
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + 1.f),     // Top-right
                    Vec3<float>(bp.x, bp.y + 1.f, bp.z + 1.f),           // Top-left
                    color);
                // Left strip (along x = bp.x, z = bp.z + 1.f)
                dc.fillQuad(Vec3<float>(bp.x, bp.y + w, bp.z + 1.f),           // Bottom-left
                    Vec3<float>(bp.x, bp.y + 1.f - w, bp.z + 1.f),     // Top-left
                    Vec3<float>(bp.x + w, bp.y + 1.f - w, bp.z + 1.f), // Top-right
                    Vec3<float>(bp.x + w, bp.y + w, bp.z + 1.f),       // Bottom-right
                    color);
                // Right strip (along x = bp.x + 1.f, z = bp.z + 1.f)
                dc.fillQuad(Vec3<float>(bp.x + 1.f - w, bp.y + w, bp.z + 1.f),     // Bottom-left
                    Vec3<float>(bp.x + 1.f - w, bp.y + 1.f - w, bp.z + 1.f), // Top-left
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z + 1.f),     // Top-right
                    Vec3<float>(bp.x + 1.f, bp.y + w, bp.z + 1.f),           // Bottom-right
                    color);
                };

            auto drawNorth = [&](float width) {
                float w = width;
                // Bottom strip (along y = bp.y, z = bp.z)
                dc.fillQuad(Vec3<float>(bp.x, bp.y, bp.z),           // Bottom-left
                    Vec3<float>(bp.x + 1.f, bp.y, bp.z),     // Bottom-right
                    Vec3<float>(bp.x + 1.f, bp.y + w, bp.z), // Top-right
                    Vec3<float>(bp.x, bp.y + w, bp.z),       // Top-left
                    color);
                // Top strip (along y = bp.y + 1.f, z = bp.z)
                dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.f - w, bp.z),     // Bottom-left
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z), // Bottom-right
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z),     // Top-right
                    Vec3<float>(bp.x, bp.y + 1.f, bp.z),           // Top-left
                    color);
                // Left strip (along x = bp.x, z = bp.z)
                dc.fillQuad(Vec3<float>(bp.x, bp.y + w, bp.z),           // Bottom-left
                    Vec3<float>(bp.x, bp.y + 1.f - w, bp.z),     // Top-left
                    Vec3<float>(bp.x + w, bp.y + 1.f - w, bp.z), // Top-right
                    Vec3<float>(bp.x + w, bp.y + w, bp.z),       // Bottom-right
                    color);
                // Right strip (along x = bp.x + 1.f, z = bp.z)
                dc.fillQuad(Vec3<float>(bp.x + 1.f - w, bp.y + w, bp.z),     // Bottom-left
                    Vec3<float>(bp.x + 1.f - w, bp.y + 1.f - w, bp.z), // Top-left
                    Vec3<float>(bp.x + 1.f, bp.y + 1.f - w, bp.z),     // Top-right
                    Vec3<float>(bp.x + 1.f, bp.y + w, bp.z),           // Bottom-right
                    color);
                };


            float width = settings.getSettingByName<float>("outlinewidth")->value;

            drawNorth(width);
            drawEast(width);
            drawSouth(width);
            drawWest(width);
            drawUp(width);
            drawDown(width);
        }
            
        else {
            auto drawUp = [&]() {
                dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z), Vec3<float>(bp.x + 1.f, bp.y + 1.f, bp.z + 1.f), Vec3<float>(bp.x, bp.y + 1.f, bp.z + 1.f), color);
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

            if (!settings.getSettingByName<bool>("overlayfullblock")->value) {
                switch (face) {
                case 0:
                    drawDown();
                    break;
                case 1: // up
                    drawUp();
                    break;
                case 2: // north
                    drawNorth();
                    break;
                case 3:
                    drawSouth();
                    break;
                case 4:
                    drawWest();
                    break;
                case 5:
                    drawEast();
                    break;
                }
            }
            else {
                drawNorth();
                drawEast();
                drawSouth();
                drawWest();
                drawUp();
                drawDown();
            }
        }

        dc.flush();

        event.cancel();
    }
};
