#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "../../../../Utils/Render/MaterialUtils.hpp"
#include "../../../Events/Game/TickEvent.hpp"
#include "../../../../SDK/Client/GUI/ScreenRenderer.hpp"


class CustomCrosshair : public Module {
private:
    Perspective currentPerspective;
public:
    CustomCrosshair() : Module("Custom Crosshair", "Allows for dynamic crosshair colors.",
                        IDR_SPEED_PNG, "") {

        Module::setup();
    };

    void onEnable() override {
        Listen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
        Listen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
        Deafen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("highlightOnEntity") == nullptr) settings.addSetting("highlightOnEntity", false);
        if (settings.getSettingByName<bool>("solidColor") == nullptr) settings.addSetting("solidColor", false);
        if (settings.getSettingByName<bool>("renderInThirdPerson") == nullptr) settings.addSetting("renderInThirdPerson", false);

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

        this->addHeader("Main");
        this->addToggle("Solid Color", "Make crosshair a solid color / more visible", settings.getSettingByName<bool>("solidColor")->value);
        this->addToggle("Render in Third Person", "Weather or not to render in third person", settings.getSettingByName<bool>("renderInThirdPerson")->value);
        this->addToggle("Highlight on Entity", "Highlight on entity", settings.getSettingByName<bool>("highlightOnEntity")->value);

        this->extraPadding();

        this->addHeader("Colors");
        this->addColorPicker("Default Color", "When the enemy is not in view.", settings.getSettingByName<std::string>("defaultColor")->value, settings.getSettingByName<float>("defaultOpacity")->value, settings.getSettingByName<bool>("defaultColorRGB")->value);
        if(settings.getSettingByName<bool>("highlightOnEntity")->value) {
            this->addColorPicker("Enemy Color", "When the enemy is in view.",
                                 settings.getSettingByName<std::string>("enemyColor")->value,
                                 settings.getSettingByName<float>("enemyOpacity")->value,
                                 settings.getSettingByName<bool>("enemyColorRGB")->value);
        }
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onGetViewPerspective(PerspectiveEvent &event) {
        currentPerspective = event.getPerspective();
    }

    void onHudCursorRendererRender(HudCursorRendererRenderEvent& event) {
        event.cancel();
        if(!SDK::clientInstance) return;

        auto renderInThirdPerson = settings.getSettingByName<bool>("renderInThirdPerson")->value;
        if(!renderInThirdPerson && currentPerspective != Perspective::FirstPerson) return;
        bool isHoveringEnemy = (SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult().type == HitResultType::Entity);

        auto screenContext = event.getMinecraftUIRenderContext()->getScreenContext();

        const ResourceLocation loc("textures/ui/cross_hair", false);
        const TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);
        const auto tess = screenContext->getTessellator();

        tess->begin(mce::PrimitiveMode::QuadList, 4);

        D2D1_COLOR_F enemyColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("enemyColor")->value);
        enemyColor.a = settings.getSettingByName<float>("enemyOpacity")->value;

        D2D1_COLOR_F defaultColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("defaultColor")->value);
        defaultColor.a = settings.getSettingByName<float>("defaultOpacity")->value;

        auto shouldHighlight = settings.getSettingByName<bool>("highlightOnEntity")->value;

        D2D1_COLOR_F color = isHoveringEnemy && shouldHighlight ? enemyColor : defaultColor;

        tess->color(color.r, color.g, color.b, color.a);

        Vec2<float> size = Vec2<float>(16, 16);

        Vec2<float> sizeScaled = PositionUtils::getScreenScaledPos(size);

        Vec2<float> pos = PositionUtils::getScaledPos(Vec2<float>((MC::windowSize.x / 2) - (sizeScaled.x / 2), (MC::windowSize.y / 2) - (sizeScaled.y / 2)));

        IntRectangle rect = IntRectangle(pos.x, pos.y, size.x, size.y);

        auto useSolidColor = settings.getSettingByName<bool>("solidColor")->value;

        auto material = isHoveringEnemy && shouldHighlight || useSolidColor ? MaterialUtils::getUITextured() : MaterialUtils::getUICrosshair() ;

        ScreenRenderer::blit(screenContext, &ptr, &rect, material);
    }
};
