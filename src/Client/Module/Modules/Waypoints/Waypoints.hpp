#pragma once

#include "../Module.hpp"


class Waypoints : public Module {
private:
    static inline std::vector<AABBInfo> aabbsToRender;
    static inline std::mutex renderMtx;
public:
    Waypoints() : Module("Waypoints", "Allows you to mark points in your world.", IDR_MAGNIFY_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &Waypoints::onRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &Waypoints::onRender)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string)"FPS: {value}");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
        if (settings.getSettingByName<float>("fpsSpoofer") == nullptr) settings.addSetting("fpsSpoofer", 1.0f);

    }

    void settingsRender(float settingsOffset) override {

    }

    void onRender(RenderEvent& event) {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
            !SDK::clientInstance->getLocalPlayer()->getLevel())
            return;

        auto player = SDK::clientInstance->getLocalPlayer();

        if (FlarialGUI::inMenu || SDK::getCurrentScreen() != "hud_screen") return;

        if (player != nullptr) {
             
            D2D1_COLOR_F color2 = FlarialGUI::rgbColor;
            
            std::lock_guard<std::mutex> guard(renderMtx);
                // Retrieve the thickness setting value from the module settings

            float lineWidth = 1.01f;

            // Get the render position of the player and compute the distance to the AABB lower bound
            float distance = player->getRenderPositionComponent()->renderPos.dist(Vec3{ 0.0F, 0.0F, 0.0F });

            // Compute the scaling factor based on the distance, ensuring it does not exceed the max distance of 30
            float scaleFactor = 1.f - (distance / 30.0f);

            // Calculate the line width, ensuring it does not fall below a minimum threshold of 0.5
            lineWidth = 1.01f * scaleFactor;

            DrawUtils::addBox(Vec3{ 0.0F, 0.0F, 0.0F }, Vec3{ 1.0F, 1.0F, 1.0F }, lineWidth, 1, color2);
            Vec2<float> screen;

            if (Matrix::WorldToScreen(Vec3{ 0.0F, 0.0F, 0.0F }, screen)) {
                FlarialGUI::FlarialTextWithFont(screen.x, screen.y, L"waypoint 1", 500, 0, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.215f, "height", true), DWRITE_FONT_WEIGHT_NORMAL, FlarialGUI::rgbColor, true);
            }
        }
    }
};
