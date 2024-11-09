#pragma once

#include "../Module.hpp"

class MovableScoreboard : public Module {
private:
    Vec2<float> currentPos{};
    bool enabled = false;
    static inline Vec2<float> originalPos = Vec2<float>{0.0f, 0.0f};
    Vec2<float> currentSize = Vec2<float>{0.0f, 0.0f};
public:
    MovableScoreboard() : Module("Movable Scoreboard", "Makes the Minecraft Scoreboard movable.", IDR_MAN_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &MovableScoreboard::onRender)
        Listen(this, SetupAndRenderEvent, &MovableScoreboard::onSetupAndRender)
        if(WinrtUtils::checkAboveOrEqual(21,40)) {
            Listen(this, UIControlGetPositionEvent, &MovableScoreboard::onUIControlGetPosition)
        }
        if (FlarialGUI::inMenu) {
            FlarialGUI::Notify("To change the position of the Scoreboard, Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &MovableScoreboard::onRender)
        Deafen(this, SetupAndRenderEvent, &MovableScoreboard::onSetupAndRender)
        if(WinrtUtils::checkAboveOrEqual(21,40)) {
            Deafen(this, UIControlGetPositionEvent, &MovableScoreboard::onUIControlGetPosition)
        }
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
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 29);

            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 29, width, height);


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

    void onUIControlGetPosition(UIControlGetPositionEvent &event) {
        auto control = event.getControl();
        if (control->getLayerName() == "sidebar") {
            if(!(currentPos == Vec2<float>{0, 0})) {
                Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
                event.setPosition(scaledPos);
            }
        }
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        if (SDK::getCurrentScreen() == "hud_screen") {
            SDK::screenView->VisualTree->root->forEachControl([this](std::shared_ptr<UIControl> &control) {
                if (control->getLayerName() == "sidebar") {
                    updatePosition(control.get());
                    return false; // dont go through other controls
                }
                return false;
            });
        }
    };

    void updatePosition(UIControl* control) {
        if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;

        auto pos = control->parentRelativePosition;

        if (originalPos.x == 0.0f) {
            originalPos = PositionUtils::getScreenScaledPos(pos);
        }

        Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

        control->parentRelativePosition = Vec2<float>{scaledPos.x, scaledPos.y};

        control->updatePosition(true);
        control->forEachChild([](std::shared_ptr<UIControl> &child) {
            child->updatePosition();
        });

        auto size = control->sizeConstrains;

        auto scaledSize = PositionUtils::getScreenScaledPos(size);

        if (scaledSize == Vec2<float>{0, 0}) {
            currentSize = PositionUtils::getScreenScaledPos(Vec2<float>{100.f, 100.0f});
            return;
        }

        currentSize = PositionUtils::getScreenScaledPos(size);

        return;
    }
};