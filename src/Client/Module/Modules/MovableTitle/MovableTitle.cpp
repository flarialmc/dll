#include "MovableTitle.hpp"

#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Utils/Render/PositionUtils.hpp"

MovableTitle::MovableTitle(): Module("Movable " + mname, "Makes the Minecraft " + mname + " movable.", IDR_MOVABLE_PNG, "")
{
    Listen(this, SetupAndRenderEvent, &MovableTitle::onSetupAndRender)
    
}

void MovableTitle::onEnable()
{
    Listen(this, RenderEvent, &MovableTitle::onRender)
    Listen(this, UIControlGetPositionEvent, &MovableTitle::onUIControlGetPosition)

    if (FlarialGUI::inMenu) {
        FlarialGUI::Notify("To change the position of the " + mname + ", Please click " +
            ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "editmenubind")->value + " in the settings tab.");
    }
    Module::onEnable();
}

void MovableTitle::onDisable()
{
    Deafen(this, RenderEvent, &MovableTitle::onRender)
    Deafen(this, UIControlGetPositionEvent, &MovableTitle::onUIControlGetPosition)

    Module::onDisable();
}

void MovableTitle::defaultConfig()
{
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    saveSettings();
}

void MovableTitle::settingsRender(float settingsOffset)
{}

void MovableTitle::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;
    auto name = SDK::getCurrentScreen();

    if (name == "hud_screen" || name == "pause_screen") {

        float width = currentSize.x;
        float height = currentSize.y;


        Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));

        if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
        else if (settingperc.x == 0 and originalPos.x != 0.0f) currentPos = Vec2<float>{ originalPos.x, originalPos.y };

        if (ClickGUI::editmenu) FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 30);

        if (currentPos.x != -120.0f)
        {
            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 30, width, height);



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

void MovableTitle::onUIControlGetPosition(UIControlGetPositionEvent& event)
{ // happens when game updates control position
    if (!this->isEnabled()) return;
    auto control = event.getControl();
    if (control->getLayerName() == layerName) {
        if (!isEnabled()) return;
        if (originalPos == Vec2<float>{0, 0}) {
            originalPos = PositionUtils::getScreenScaledPos(control->parentRelativePosition);
            return;
        }
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

void MovableTitle::onSetupAndRender(SetupAndRenderEvent& event)
{
    if (!this->isEnabled()) return;
    update();
}

void MovableTitle::update()
{
    if (ClickGUI::editmenu) {
        if (!isEnabled()) return;
    }
    else {
        if (lastAppliedPos == (isEnabled() ? currentPos : originalPos)) return;
    }
    if (SDK::getCurrentScreen() != "hud_screen") return;
    SDK::screenView->VisualTree->root->forEachControl([this](std::shared_ptr<UIControl>& control) {
        if (control->getLayerName() == layerName) {
            updatePosition(control.get());
            return true; // dont go through other controls
        }
        return false;
    });
}

void MovableTitle::updatePosition(UIControl* control)
{
    if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;

    auto pos = control->parentRelativePosition;

    if (isEnabled() && originalPos == Vec2<float>{0, 0}) {
        originalPos = PositionUtils::getScreenScaledPos(pos);
    }

    Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
    Vec2<float> scaledOriginalPos = PositionUtils::getScaledPos(originalPos);

    control->parentRelativePosition = isEnabled() ? scaledPos : scaledOriginalPos;
    lastAppliedPos = isEnabled() ? currentPos : originalPos;
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
