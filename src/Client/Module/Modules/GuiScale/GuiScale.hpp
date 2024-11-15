#pragma once

#include "../Module.hpp"

class GuiScale : public Module {
private:

public:

    float originalScale = 0.f;
    float lastAppliedScale = 0.f;
    int troll = 0;
    bool firstTime = true;
    static inline bool troller = false;

    GuiScale() : Module("MC GUI Scale", "Change your GUI Scale beyond\nMinecraft's restrictions.",
                        IDR_SCALE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, SetupAndRenderEvent, &GuiScale::onSetupAndRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, SetupAndRenderEvent, &GuiScale::onSetupAndRender)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("guiscale") == nullptr) settings.addSetting("guiscale", 2.0f);
    }

    void settingsRender(float settingsOffset) override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);


        float percent = FlarialGUI::Slider(4, toggleX + FlarialGUI::SettingsTextWidth("UI Scale "),
                                           toggleY, this->settings.getSettingByName<float>("guiscale")->value, 4.0f);

        this->settings.getSettingByName<float>("guiscale")->value = percent;

    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        update();
    };

    void update() {
        float currentScale = this->settings.getSettingByName<float>("guiscale")->value;
        float targetScale = isEnabled() ? currentScale : originalScale;
        if((troller || firstTime) && SDK::getCurrentScreen() == "hud_screen") {
            if(troll > 150) {
                troller = false;
                firstTime = false;
                troll = 0;
            } else {
                troll++;
            }
            updateScale(targetScale);
            return;
        }
        if(lastAppliedScale == targetScale) return;
        if(SDK::getCurrentScreen() != "hud_screen") return;


        updateScale(targetScale);
    }

    void updateScale(float newScale) {
        lastAppliedScale = newScale;

        auto guiData = SDK::clientInstance->getGuiData();
        if(originalScale == 0) {
            originalScale = SDK::clientInstance->getGuiData()->GuiScale;
        }
        float oldScale = guiData->GuiScale;
        guiData->GuiScale = newScale;
        guiData->ScreenSizeScaled = Vec2 {
                guiData->ScreenSize.x * 1 / newScale,
                guiData->ScreenSize.y * 1 / newScale
        };
        guiData->scalingMultiplier = 1 / newScale;

        auto scaledSize = guiData->ScreenSizeScaled;
        auto centerScaled = Vec2 { scaledSize.x / 2, scaledSize.y / 2 };

        auto movableBossbar = ModuleManager::getModule("Movable Bossbar");
        auto movableScoreboard = ModuleManager::getModule("Movable Scoreboard");
        auto movableHotbar = ModuleManager::getModule("Movable Hotbar");

        // this is performance hell
        SDK::screenView->VisualTree->root->forEachControl([&centerScaled, &scaledSize, &movableBossbar, &movableScoreboard, &movableHotbar](std::shared_ptr<UIControl> &control) {
            if(control->getLayerName() == "boss_health_grid"){
                if(movableBossbar && movableBossbar->isEnabled()) return false;
                control->parentRelativePosition = Vec2<float> { centerScaled.x - (control->sizeConstrains.x / 2), scaledSize.y };
                if(WinrtUtils::checkAboveOrEqual(21,40)) {
                    control->updatePosition(true);
                }
                control->forEachChild([](std::shared_ptr<UIControl> &child) {
                    child->updatePosition();
                });
                return false;
            }
            if(control->getLayerName() == "sidebar"){
                if(movableScoreboard && movableScoreboard->isEnabled()) return false;
                control->parentRelativePosition = Vec2<float> { scaledSize.x - control->sizeConstrains.x, centerScaled.y - (control->sizeConstrains.y / 2) };
                if(WinrtUtils::checkAboveOrEqual(21,40)) {
                    control->updatePosition(true);
                }
                control->forEachChild([](std::shared_ptr<UIControl> &child) {
                    child->updatePosition();
                });
                return false;
            }
            if(control->getLayerName() == "centered_gui_elements_at_bottom_middle"){
                if(movableHotbar && movableHotbar->isEnabled()) return false;
                control->parentRelativePosition = Vec2<float> { centerScaled.x - (control->sizeConstrains.x / 2), scaledSize.y - control->sizeConstrains.y };
                if(WinrtUtils::checkAboveOrEqual(21,40)) {
                    control->updatePosition(true);
                }
                control->forEachChild([](std::shared_ptr<UIControl> &child) {
                    child->updatePosition();
                });
                return false;
            }

            return false;
        });
    }
};
