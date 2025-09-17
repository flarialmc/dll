#include "MovableCoordinates.hpp"
#include "SDK/Client/Render/GuiData.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "SDK/Client/Core/Options.hpp"
#include "Utils/Render/PositionUtils.hpp"


void MovableCoordinates::onEnable() {
    restored = false;
    Listen(this, SetupAndRenderEvent, &MovableCoordinates::onSetupAndRender)
    Listen(this, RenderEvent, &MovableCoordinates::onRender)
    Listen(this, UIControlGetPositionEvent, &MovableCoordinates::onUIControlGetPosition)
    Module::onEnable();
}

void MovableCoordinates::onDisable() {
    Module::onDisable();
    if (!restored) {
        delayDisable = true;
        return;
    }
    Deafen(this, SetupAndRenderEvent, &MovableCoordinates::onSetupAndRender)
    Deafen(this, RenderEvent, &MovableCoordinates::onRender)
    Deafen(this, UIControlGetPositionEvent, &MovableCoordinates::onUIControlGetPosition)
}

void MovableCoordinates::defaultConfig() {
    Module::defaultConfig("core");
    setDef("percentageX", 0.f);
    setDef("percentageY", 0.f);
}

void MovableCoordinates::settingsRender(float settingsOffset) {
}

void MovableCoordinates::onRender(RenderEvent &event) {
    if (!this->isEnabled() && !delayDisable) return;
    auto name = SDK::getCurrentScreen();

    if (name == "hud_screen" || name == "pause_screen") {
        float width = currentSize.x;
        float height = currentSize.y;


        Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"),
                                              getOps<float>("percentageY"));

        if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
        else if (settingperc.x == 0 and originalPos.x != 0.0f) currentPos = Vec2<float>{originalPos.x, originalPos.y};

        if (ClickGUI::editmenu) {
            FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 27, this->name);
            checkForRightClickAndOpenSettings(currentPos.x, currentPos.y, width, height);
        }

        if (currentPos.x != -120.0f) {
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

void MovableCoordinates::onUIControlGetPosition(UIControlGetPositionEvent &event) {
    // happens when game updates control position
    if (!enabledState && !delayDisable) return;
    auto control = event.getControl();
    if (control->getLayerName() == layerName) {
        Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
        if (event.getPosition() == nullptr) {
            // 1.21.30 and below
            control->parentRelativePosition = scaledPos;
            control->forEachChild([](std::shared_ptr<UIControl> &child) {
                child->updatePosition();
            });
            return;
        };
        if (*event.getPosition() == PositionUtils::getScaledPos(currentPos)) return;
        event.setPosition(scaledPos);
    }
}

void MovableCoordinates::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled() && !delayDisable) return;
    update();
}

void MovableCoordinates::update() {
    if (restored) return;
    if (!delayDisable) {
        if (ClickGUI::editmenu) {
            if (!enabledState) return;
        } else {
            if (lastAppliedPos == (enabledState ? currentPos : originalPos)) return;
        }
        if (SDK::getCurrentScreen() != "hud_screen") return;
    }
    SDK::screenView->VisualTree->root->forEachChild([this](std::shared_ptr<UIControl> &control) {
        if (control->getLayerName() == layerName) {
            updatePosition(control.get());
            return true; // dont go through other controls
        }
        return false;
    });

    if (delayDisable) {
        delayDisable = false;
        restored = true;
    }
}

void MovableCoordinates::updatePosition(UIControl *control) {
    if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;

    if ((enabledState && originalPos == Vec2<float>{0, 0}) || delayDisable) {
        auto guiData = SDK::clientInstance->getGuiData();
        auto scaledSize = guiData->ScreenSizeScaled;
        auto recalculatedPos = Vec2<float>{0.f, 50.f}; // default coords position
        originalPos = PositionUtils::getScreenScaledPos(recalculatedPos);
        currentPos = originalPos;
    }

    Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
    Vec2<float> scaledOriginalPos = PositionUtils::getScaledPos(originalPos);

    control->parentRelativePosition = enabledState ? scaledPos : scaledOriginalPos;
    lastAppliedPos = enabledState ? currentPos : originalPos;
    if (VersionUtils::checkAboveOrEqual(21, 40)) {
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

    if (_scaledSize.x < 10) _scaledSize.x = 10;

    if (_scaledSize.y < 10) _scaledSize.y = 10;

    currentSize = _scaledSize;
}
