#pragma once

#include "../Module.hpp"

class MovableBossbar : public Module {
private:
    static inline std::string layerName = "boss_health_grid";
    Vec2<float> currentPos{};
    bool enabled = false;
    static inline Vec2<float> originalPos = Vec2<float>{0.0f, 0.0f};
    Vec2<float> currentSize = Vec2<float>{0.0f, 0.0f};
    Vec2<float> lastAppliedPos = Vec2<float>{0.0f, 0.0f};
public:
    static inline std::string name = "Bossbar";

    MovableBossbar() : Module("Movable " + name, "Makes the Minecraft " + name + " movable.", IDR_MOVABLE_PNG, "") {
        Listen(this, SetupAndRenderEvent, &MovableBossbar::onSetupAndRender)
        Module::setup();
    };

    void onEnable() override {
        originalPos = Vec2<float>{0, 0};
        Listen(this, RenderEvent, &MovableBossbar::onRender)
        Listen(this, UIControlGetPositionEvent, &MovableBossbar::onUIControlGetPosition)

        if (FlarialGUI::inMenu) {
            FlarialGUI::Notify("To change the position of the " + name +", Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &MovableBossbar::onRender)
        Deafen(this, UIControlGetPositionEvent, &MovableBossbar::onUIControlGetPosition)

        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
        }
        if (settings.getSettingByName<float>("percentageY") == nullptr) {
            settings.addSetting("percentageY", 0.0f);
        }
    }

    void settingsRender(float settingsOffset) override {}

    void onRender(RenderEvent &event) {

        auto name = SDK::getCurrentScreen();

        if (name == "hud_screen" || name == "pause_screen") {

            float width = currentSize.x;
            float height = currentSize.y;


            Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
                                                  this->settings.getSettingByName<float>("percentageY")->value);

            if (settingperc.x != 0)
                currentPos = Vec2<float>(settingperc.x * MC::windowSize.x,
                                         settingperc.y * MC::windowSize.y);
            else if (settingperc.x == 0 and originalPos.x != 0.0f)
                currentPos = Vec2<float>{originalPos.x, originalPos.y};

            if (ClickGUI::editmenu)
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 25);

            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 25, width, height);


            currentPos.x = vec2.x;
            currentPos.y = vec2.y;

            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y);

            this->settings.setValue("percentageX", percentages.x);
            this->settings.setValue("percentageY", percentages.y);

            if (ClickGUI::editmenu) {
                FlarialGUI::RoundedRect(currentPos.x, currentPos.y, D2D1::ColorF(D2D1::ColorF::White, 0.2f), width,
                                        height);
                FlarialGUI::UnsetWindowRect();
            }
        }
    }

    void onUIControlGetPosition(UIControlGetPositionEvent &event) { // happens when game updates control position
        auto control = event.getControl();
        if (control->getLayerName() == layerName) {
            if(!isEnabled()) return;
            Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
            if(event.getPosition() == nullptr) { // 1.21.30 and below
                control->parentRelativePosition = scaledPos;
                control->forEachChild([](std::shared_ptr<UIControl> &child) {
                    child->updatePosition();
                });
                return;
            };
            if(*event.getPosition() == PositionUtils::getScaledPos(currentPos)) return;

            event.setPosition(scaledPos);
        }
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        update();
    };

    void update() {
        if(ClickGUI::editmenu) {
            if (!isEnabled()) return;
        } else {
            if (lastAppliedPos != Vec2<float>{0, 0} && lastAppliedPos == (isEnabled() ? currentPos : originalPos)) return;
        }
        if(SDK::getCurrentScreen() != "hud_screen") return;
        SDK::screenView->VisualTree->root->forEachControl([this](std::shared_ptr<UIControl> &control) {
            if (control->getLayerName() == layerName) {
                updatePosition(control.get());
                return true; // dont go through other controls
            }
            return false;
        });
    }

    void updatePosition(UIControl* control) {
        if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;

        auto pos = control->parentRelativePosition;

        if (isEnabled() && originalPos == Vec2<float>{0, 0}) {
            auto guiData = SDK::clientInstance->getGuiData();
            auto scaledSize = guiData->ScreenSizeScaled;
            auto centerScaled = Vec2 { scaledSize.x / 2, scaledSize.y / 2 };
            auto recalculatedPos = Vec2<float>{ centerScaled.x - (control->sizeConstrains.x / 2), scaledSize.y };
            originalPos = PositionUtils::getScreenScaledPos(recalculatedPos);
            currentPos = originalPos;
        }

        Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
        Vec2<float> scaledOriginalPos = PositionUtils::getScaledPos(originalPos);

        control->parentRelativePosition = isEnabled() ? scaledPos : scaledOriginalPos;
        lastAppliedPos = isEnabled() ? currentPos : originalPos;
        if(WinrtUtils::checkAboveOrEqual(21,40)) {
            control->updatePosition(true);
        }
        control->forEachChild([](std::shared_ptr<UIControl> &child) {
            child->updatePosition();
        });

        auto size = control->sizeConstrains;

        auto scaledSize = PositionUtils::getScreenScaledPos(size);

        if (scaledSize == Vec2<float>{0, 0}) {
            currentSize = PositionUtils::getScreenScaledPos(Vec2<float>{100.f, 100.0f});
            return;
        }

        auto _scaledSize = PositionUtils::getScreenScaledPos(size);

        if(_scaledSize.x < 10)
            _scaledSize.x = 10;

        if(_scaledSize.y < 10)
            _scaledSize.y = 10;

        currentSize = _scaledSize;

        return;
    }
};