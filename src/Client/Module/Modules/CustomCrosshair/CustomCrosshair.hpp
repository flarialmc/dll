#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "../../../../Utils/Render/MaterialUtils.hpp"
#include "../../../Events/Game/TickEvent.hpp"


class CustomCrosshair : public Module {
public:
    CustomCrosshair() : Module("Custom Crosshair", "Allows for dynamic crosshair colors.",
                        IDR_SPEED_PNG, "") {

        Module::setup();
    };

    static inline bool isHoveringEnemy = false;

    void onEnable() override {
        Listen(this, TickEvent, &CustomCrosshair::onTick)
        Listen(this, SetupAndRenderEvent, &CustomCrosshair::onSetupAndRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, TickEvent, &CustomCrosshair::onTick)
        Deafen(this, SetupAndRenderEvent, &CustomCrosshair::onSetupAndRender)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("defaultColor") == nullptr) settings.addSetting("defaultColor", (std::string) "fafafa");
        if (settings.getSettingByName<bool>("defaultColorRGB") == nullptr) settings.addSetting("defaultColorRGB", false);
        if (settings.getSettingByName<float>("defaultOpacity") == nullptr) settings.addSetting("defaultOpacity", 0.55f);

        if (settings.getSettingByName<std::string>("enemyColor") == nullptr) settings.addSetting("enemyColor", (std::string) "FF0000");
        if (settings.getSettingByName<bool>("enemyColorRGB") == nullptr) settings.addSetting("enemyColorRGB", false);
        if (settings.getSettingByName<float>("enemyOpacity") == nullptr) settings.addSetting("enemyOpacity", 0.55f);
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
        this->addColorPicker("Default Color", "When the enemy is not in view.", settings.getSettingByName<std::string>("defaultColor")->value, settings.getSettingByName<float>("defaultOpacity")->value, settings.getSettingByName<bool>("defaultColorRGB")->value);
        this->addColorPicker("Enemy Color", "When the enemy is in view.", settings.getSettingByName<std::string>("enemyColor")->value, settings.getSettingByName<float>("enemyOpacity")->value, settings.getSettingByName<bool>("enemyColorRGB")->value);
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onTick(TickEvent& event) {
        isHoveringEnemy = (SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult().type == HitResultType::Entity);
    }

    void onSetupAndRender(SetupAndRenderEvent& event) {

        ScreenContext* screenContext = event.getMuirc()->getScreenContext();
        const ResourceLocation loc("textures/ui/cross_hair", false);
        const TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);
        const auto tess = screenContext->getTessellator();
        tess->begin();

        D2D1_COLOR_F enemyColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("enemyColor")->value);
        enemyColor.a = settings.getSettingByName<float>("enemyOpacity")->value;

        D2D1_COLOR_F defaultColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("defaultColor")->value);
        defaultColor.a = settings.getSettingByName<float>("defaultOpacity")->value;

        D2D1_COLOR_F color = isHoveringEnemy ? enemyColor : defaultColor;
        tess->color(color.r, color.g, color.b, color.a);

        Vec2<float> size = Vec2(50.f, 50.f);
        Vec2<float> scaledSize = PositionUtils::getScreenScaledPos(size);
        Vec2<float> pos = PositionUtils::getScaledPos(Vec2<float>((MC::windowSize.x / 2) - size.x, (MC::windowSize.y / 2) - size.y));

        tess->vertexUV(pos.x, pos.y, 0.f, 0.f, 0.f);
        tess->vertexUV(pos.x, pos.y + size.x, 0.f, 0.f, 1.f);
        tess->vertexUV(pos.x + size.x, pos.y, 0.f, 1.f, 0.f);

        tess->vertexUV(pos.x, pos.y + size.x, 0.f, 0.f, 1.f);
        tess->vertexUV(pos.x + size.x, pos.y + size.y, 0.f, 1.f, 1.f);
        tess->vertexUV(pos.x + size.x, pos.y, 0.f, 1.f, 0.f);

        MeshHelpers::renderMeshImmediately2(screenContext, tess, MaterialUtils::getUITextured(), *ptr.clientTexture);

    }
};
