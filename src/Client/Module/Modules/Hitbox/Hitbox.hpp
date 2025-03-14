#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"

struct AABBInfo {
    AABB aabb;
    AABB hitbox;
    bool selected;
};

class Hitbox : public Module {
private:
    static inline std::vector<AABBInfo> aabbsToRender;
    static inline std::mutex renderMtx;
public:

    Hitbox() : Module("Hitbox", "Displays hitboxes of entities", IDR_BLOCK_PNG, "") {

        Module::setup();

    };

    // TODO: add options to see all entities

    void onEnable() override {
        Listen(this, SetupAndRenderEvent, &Hitbox::onSetupAndRender)
        Listen(this, RenderEvent, &Hitbox::onRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, SetupAndRenderEvent, &Hitbox::onSetupAndRender)
        Deafen(this, RenderEvent, &Hitbox::onRender)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.6f);
        if (settings.getSettingByName<float>("thickness") == nullptr) settings.addSetting("thickness", 1.1f);
        if (settings.getSettingByName<bool>("staticThickness") == nullptr) settings.addSetting("staticThickness", false);
        if (settings.getSettingByName<bool>("outline") == nullptr) settings.addSetting("outline", false);

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

        this->addToggle("2D Mode", "", this->settings.getSettingByName<bool>("outline")->value);
        this->addToggle("Static Thickness", "", this->settings.getSettingByName<bool>("staticThickness")->value);

        this->addSlider("Thickness", "", this->settings.getSettingByName<float>("thickness")->value);

        this->extraPadding();
        this->addHeader("Colors");
        this->addColorPicker("Hitbox Color", "", settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        std::lock_guard<std::mutex> guard(renderMtx);
        aabbsToRender.clear();
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
            !SDK::clientInstance->getLocalPlayer()->getLevel())
            return;
        auto player = SDK::clientInstance->getLocalPlayer();
        auto selectedEntity = player->getLevel()->getHitResult().getEntity();
        for (const auto &ent: player->getLevel()->getRuntimeActorList()) {
            auto pos = ent->getPosition();
            if(!pos) continue;
            if (ent != nullptr && ent != player && ent->hasCategory(ActorCategory::Mob)/*&& ent->isPlayer() && ent->hasCategory(ActorCategory::Player)*/) {
                float dist = player->getPosition()->dist(*pos);
                // This may let through some entites
                if (!ent->isValid() || !ent->isValidAABB() || dist > 30 || !player->canSee(*ent) ||
                    ent->getActorFlag(ActorFlags::FLAG_INVISIBLE))
                    continue;

                auto aabb = ent->getLerpedAABB();
                auto hitbox = ent->getLerpedAABB(true);

                AABBInfo info = {aabb, hitbox, selectedEntity == ent};

                aabbsToRender.emplace_back(info);
            }
        }
    }

    void onRender(RenderEvent &event) {

        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
            !SDK::clientInstance->getLocalPlayer()->getLevel())
            return;

        auto player = SDK::clientInstance->getLocalPlayer();

        if(FlarialGUI::inMenu || SDK::getCurrentScreen() != "hud_screen") return;

        if (player != nullptr) {
            D2D1_COLOR_F color2;
            // TODO: optimize getting colors
            if (this->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
            else color2 = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color")->value);

            std::lock_guard<std::mutex> guard(renderMtx);
            for (const auto &aabbInfo: aabbsToRender) {
                // Retrieve the thickness setting value from the module settings
                float thickness = this->settings.getSettingByName<float>("thickness")->value;
                bool staticThickness = this->settings.getSettingByName<bool>("staticThickness")->value;
                bool outline = this->settings.getSettingByName<bool>("outline")->value;

                float lineWidth = thickness;

                if(!staticThickness) {
                    // Get the render position of the player and compute the distance to the AABB lower bound
                    float distance = player->getRenderPositionComponent()->renderPos.dist(aabbInfo.aabb.lower);

                    // Compute the scaling factor based on the distance, ensuring it does not exceed the max distance of 30
                    float scaleFactor = 1.f - (distance / 30.0f);

                    // Calculate the line width, ensuring it does not fall below a minimum threshold of 0.5
                    lineWidth = thickness * scaleFactor;
                }

                DrawUtils::addBox(aabbInfo.aabb.lower, aabbInfo.aabb.upper, staticThickness ? thickness : lineWidth, outline ? 2 : 1, color2);

                //DrawUtils::addBox(aabbInfo.hitbox.lower, aabbInfo.hitbox.upper, staticThickness ? thickness : lineWidth, outline ? 2 : 1, aabbInfo.selected ? FlarialGUI::rgbColor :color2);
            }
        }
    }
};
