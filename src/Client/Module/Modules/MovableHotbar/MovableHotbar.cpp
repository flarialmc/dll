#include "MovableHotbar.hpp"

MovableHotbar::MovableHotbar(): Module("Movable " + mname, "Makes the Minecraft " + mname + " movable.", IDR_MOVABLE_PNG, "")
{
    Module::setup();
}

void MovableHotbar::onEnable()
{
    restored = false;
    Listen(this, SetupAndRenderEvent, &MovableHotbar::onSetupAndRender)
    Listen(this, RenderEvent, &MovableHotbar::onRender)
    Listen(this, UIControlGetPositionEvent, &MovableHotbar::onUIControlGetPosition)

    if (FlarialGUI::inMenu) {
        FlarialGUI::Notify("To change the position of the " + mname + ", Please click " +
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "editmenubind")->value + " in the settings tab.");
    }
    Module::onEnable();
}

void MovableHotbar::onDisable()
{
    if (!restored) {
        delayDisable = true;
        return;
    }
    Deafen(this, SetupAndRenderEvent, &MovableHotbar::onSetupAndRender)
    Deafen(this, RenderEvent, &MovableHotbar::onRender)
    Deafen(this, UIControlGetPositionEvent, &MovableHotbar::onUIControlGetPosition)

    Module::onDisable();
}

void MovableHotbar::defaultConfig()
{
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
}

void MovableHotbar::settingsRender(float settingsOffset)
{}

void MovableHotbar::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;
    auto name = SDK::getCurrentScreen();

    if (name == "hud_screen" || name == "pause_screen") {

        float width = currentSize.x;
        float height = currentSize.y;


        Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"),
                                              getOps<float>("percentageY"));

        if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
        else if (settingperc.x == 0 and originalPos.x != 0.0f) currentPos = Vec2<float>{ originalPos.x, originalPos.y };

        if (ClickGUI::editmenu) FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 28);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 28, width, height);

        if (currentPos.x != -120.0f)
        {
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

void MovableHotbar::onUIControlGetPosition(UIControlGetPositionEvent& event) const
{ // happens when game updates control position
    if (!enabledState) return;
    auto control = event.getControl();
    if (control->getLayerName() == layerName) {
        Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
        if (event.getPosition() == nullptr) { // 1.21.30 and below
            control->parentRelativePosition = scaledPos;
            control->forEachChild([](std::shared_ptr<UIControl>& child) {
                child->updatePosition();
            });
            return;
        };
        if (*event.getPosition() == PositionUtils::getScaledPos(currentPos)) return;

        event.setPosition(scaledPos);
    }
}

void MovableHotbar::onSetupAndRender(SetupAndRenderEvent& event)
{
    if (!this->isEnabled()) return;
    update();
}

void MovableHotbar::update()
{
    if (restored) return;
    if (!delayDisable) {
        if (ClickGUI::editmenu) {
            if (!enabledState) return;
        }
        else {
            if (lastAppliedPos == (enabledState ? currentPos : originalPos)) return;
        }
        if (SDK::getCurrentScreen() != "hud_screen") return;
    }
    SDK::screenView->VisualTree->root->forEachChild([this](std::shared_ptr<UIControl>& control) {
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

void MovableHotbar::updatePosition(UIControl* control)
{
    if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;

    auto pos = control->parentRelativePosition;

    if (enabledState && originalPos == Vec2<float>{0, 0}) {
        auto guiData = SDK::clientInstance->getGuiData();
        auto scaledSize = guiData->ScreenSizeScaled;
        auto centerScaled = Vec2{ scaledSize.x / 2, scaledSize.y / 2 };
        auto recalculatedPos = Vec2<float>{ centerScaled.x - (control->sizeConstrains.x / 2), scaledSize.y - control->sizeConstrains.y };
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
    control->forEachChild([](std::shared_ptr<UIControl>& child) {
        child->updatePosition();
    });

    auto size = control->sizeConstrains;

    auto scaledSize = PositionUtils::getScreenScaledPos(size);

    if (scaledSize == Vec2<float>{0, 0}) {
        currentSize = PositionUtils::getScreenScaledPos(Vec2<float>{100.f, 100.0f});
        return;
    }

    auto _scaledSize = PositionUtils::getScreenScaledPos(size);

    if (_scaledSize.x < 10)
        _scaledSize.x = 10;

    if (_scaledSize.y < 10)
        _scaledSize.y = 10;

    currentSize = _scaledSize;

    return;
}
