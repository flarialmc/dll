#include "PaperDoll.hpp"

#include "Events/EventManager.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Utils/Render/PositionUtils.hpp"

PaperDoll::PaperDoll(): Module("Movable Paperdoll", "Makes the Minecraft paperdoll movable.", IDR_MAN_PNG,
                               "") {
}

void PaperDoll::onEnable() {
    restored = false;
    Listen(this, RenderEvent, &PaperDoll::onRender)
    Listen(this, SetupAndRenderEvent, &PaperDoll::onSetupAndRender)
    if (FlarialGUI::inMenu) {
        FlarialGUI::Notify("To change the position of the Paperdoll, Please click " +
                           ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                               "editmenubind")->value + " in the settings tab.");
    }
    Module::onEnable();
}

void PaperDoll::onDisable() {
    Module::onDisable();
    if (!restored) {
        delayDisable = true;
        return;
    }
    Deafen(this, RenderEvent, &PaperDoll::onRender)
    Deafen(this, SetupAndRenderEvent, &PaperDoll::onSetupAndRender)
}

void PaperDoll::defaultConfig() {
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    setDef("uiscale", 21.0f);
    setDef("alwaysshow", false);
}

void PaperDoll::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Movable Paperdoll");
    addSlider("UI Scale", "", "uiscale");
    addToggle("Always Show", "", "alwaysshow");
    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void PaperDoll::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (ClientInstance::getTopScreenName() == "hud_screen" &&
        this->isEnabled() ||
        ClientInstance::getTopScreenName() == "pause_screen" &&
        this->isEnabled()) {
        float width = currentSize.x;
        float height = currentSize.y;


        Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"),
                                              getOps<float>("percentageY"));

        if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
        else if (settingperc.x == 0 and originalPos.x != 0.0f) currentPos = Vec2<float>{originalPos.x, originalPos.y};

        if (ClickGUI::editmenu) {
            FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 19, this->name);
            checkForRightClickAndOpenSettings(currentPos.x, currentPos.y, width, height);
        }


        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 19, width, height);


        currentPos.x = vec2.x;
        currentPos.y = vec2.y;

        Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        if (ClickGUI::editmenu) {
            FlarialGUI::UnsetWindowRect();
        }
    }
}

void PaperDoll::onSetupAndRender(SetupAndRenderEvent &event) {
    if (this->isEnabled() || delayDisable)
        if (SDK::getCurrentScreen() == "hud_screen") {
            SDK::screenView->VisualTree->root->forEachControl([this](std::shared_ptr<UIControl> &control) {
                if (control->getLayerName() == "hud_player") {
                    auto pos = control->parentRelativePosition;

                    if (originalPos.x == 0.0f) {
                        originalPos = PositionUtils::getScreenScaledPos(pos);
                    }

                    Vec2<float> scaledPos = PositionUtils::getScaledPos((!this->isEnabled() && delayDisable) ? originalPos : currentPos);

                    control->parentRelativePosition = Vec2<float>{scaledPos.x, scaledPos.y};

                    auto scale = getOps<float>("uiscale");

                    auto size = Vec2<float>{scale, scale * 2};

                    currentSize = PositionUtils::getScreenScaledPos(size);

                    control->sizeConstrains = Vec2<float>{scale, scale};

                    if (getOps<bool>("alwaysshow") || ClickGUI::editmenu) {
                        auto component = reinterpret_cast<CustomRenderComponent *>(control->getComponents()[4].get());
                        component->renderer->state = 1.0f;
                    }

                    if (delayDisable) {
                        delayDisable = false;
                        restored = true;
                    }

                    return true; // dont go through other controls
                }
                return false;
            });
        }
}
