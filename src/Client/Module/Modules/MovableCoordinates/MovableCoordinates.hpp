#pragma once

#include "../Module.hpp"

class MovableCoordinates : public Module {
private:
    static inline std::string layerName = "player_position";
    Vec2<float> currentPos{-120.0f, -120.0f};;
    bool enabled = false;
    static inline Vec2<float> originalPos = Vec2<float>{0.0f, 0.0f};
    Vec2<float> currentSize = Vec2<float>{0.0f, 0.0f};
    Vec2<float> lastAppliedPos = Vec2<float>{0.0f, 0.0f};
    bool restored = false;
public:
    static inline std::string mname = "Coordinates"; // dont use "name" var pls cuz it will override this->name so Module("name here") would be useless

    MovableCoordinates() : Module("Movable " + mname, "Makes the Minecraft " + mname + " movable.", IDR_MOVABLE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        restored = false;
        Listen(this, SetupAndRenderEvent, &MovableCoordinates::onSetupAndRender)
        Listen(this, RenderEvent, &MovableCoordinates::onRender)
        Listen(this, UIControlGetPositionEvent, &MovableCoordinates::onUIControlGetPosition)

        if (FlarialGUI::inMenu) {
            FlarialGUI::Notify("To change the position of the " + mname +", Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
    }

    void onDisable() override {
        if(!restored) {
            delayDisable = true;
            return;
        }
        Deafen(this, SetupAndRenderEvent, &MovableCoordinates::onSetupAndRender)
        Deafen(this, RenderEvent, &MovableCoordinates::onRender)
        Deafen(this, UIControlGetPositionEvent, &MovableCoordinates::onUIControlGetPosition)

        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
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
        currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
            else if (settingperc.x == 0 and originalPos.x != 0.0f)
                currentPos = Vec2<float>{originalPos.x, originalPos.y};

            if (ClickGUI::editmenu)
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 27);

            if (currentPos.x != -120.0f)
            {
                Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 27, width, height);


                currentPos.x = vec2.x;
                currentPos.y = vec2.y;

                Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

                this->settings.setValue("percentageX", percentages.x);
                this->settings.setValue("percentageY", percentages.y);
            }

            if (ClickGUI::editmenu) {
                FlarialGUI::UnsetWindowRect();
            }
        }
    }

    void onUIControlGetPosition(UIControlGetPositionEvent &event) { // happens when game updates control position
        auto control = event.getControl();
        if (control->getLayerName() == layerName) {
            if(!enabledState) return;
            if (originalPos == Vec2<float>{0, 0}) {
                originalPos = PositionUtils::getScreenScaledPos(control->parentRelativePosition);
                return;
            }
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
        if(restored) return;
        if(!delayDisable) {
            if (ClickGUI::editmenu) {
                if (!enabledState) return;
            } else {
                if (lastAppliedPos == (enabledState ? currentPos : originalPos)) return;
            }
            if(SDK::getCurrentScreen() != "hud_screen") return;
        }
        if(SDK::getCurrentScreen() != "hud_screen") return;
        SDK::screenView->VisualTree->root->forEachChild([this](std::shared_ptr<UIControl> &control) {
            if (control->getLayerName() == layerName) {
                updatePosition(control.get());
                return true; // dont go through other controls
            }
            return false;
        });

        if(delayDisable) {
            delayDisable = false;
            restored = true;
        }
    }

    void updatePosition(UIControl* control) {
        if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;

        auto pos = control->parentRelativePosition;

        if (enabledState && originalPos == Vec2<float>{0, 0}) {
            originalPos = PositionUtils::getScreenScaledPos(pos);
            currentPos = originalPos;
        }

        Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
        Vec2<float> scaledOriginalPos = PositionUtils::getScaledPos(originalPos);

        control->parentRelativePosition = enabledState ? scaledPos : scaledOriginalPos;
        lastAppliedPos = enabledState ? currentPos : originalPos;
        if(VersionUtils::checkAboveOrEqual(21,40)) {
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