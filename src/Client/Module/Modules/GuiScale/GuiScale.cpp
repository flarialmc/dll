#include "GuiScale.hpp"

#include "Events/EventManager.hpp"
#include "Modules/MovableBossbar/MovableBossbar.hpp"
#include "Modules/MovableChat/MovableChat.hpp"
#include "Modules/MovableCoordinates/MovableCoordinates.hpp"
#include "Modules/MovableDayCounter/MovableDayCounter.hpp"
#include "Modules/MovableHotbar/MovableHotbar.hpp"
#include "Modules/MovableScoreboard/MovableScoreboard.hpp"

void GuiScale::onEnable() {
    restored = false;
    Listen(this, SetupAndRenderEvent, &GuiScale::onSetupAndRender)
    Module::onEnable();
}

void GuiScale::onDisable() {
    Module::onDisable();
    if (!restored) {
        delayDisable = true;
        return;
    }
    Deafen(this, SetupAndRenderEvent, &GuiScale::onSetupAndRender)
}

void GuiScale::defaultConfig() {
    Module::defaultConfig("core");
    setDef("guiscale", 2.f);
}

void GuiScale::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("GUI Scale");
    addSlider("UI Scale", "", "guiscale", 4.f, 1.f, false);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void GuiScale::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled() && !delayDisable) return;
    update();
}

void GuiScale::update() {
    float targetScale = delayDisable ? originalScale : getOps<float>("guiscale");
    auto guiData = SDK::clientInstance->getGuiData();
    if (targetScale == guiData->GuiScale && !delayDisable && !fixResize) return;
    updateScale(targetScale);
}

void GuiScale::updateScale(float newScale) {
    if (restored && !fixResize) return;

    fixResize = false;
    auto guiData = SDK::clientInstance->getGuiData();

    if (originalScale == 0) originalScale = guiData->GuiScale;
    if (newScale == 0) newScale = getOps<float>("guiscale");

    float oldScale = guiData->GuiScale;

    auto screenSize = guiData->ScreenSize;
    static auto safeZone = Vec2<float>{0.f, 0.f};

    SDK::clientInstance->_updateScreenSizeVariables(&screenSize, &safeZone, newScale < 1.f ? 1.f : newScale);
    SDK::screenView->VisualTree->root->forEachChild([this](std::shared_ptr<UIControl> &control) {
        control->updatePosition();
    });

    if (auto movableHotbar = ModuleManager::getModule("Movable Hotbar"); movableHotbar && movableHotbar->isEnabled()) {
        if (std::shared_ptr<MovableHotbar> mod = std::dynamic_pointer_cast<MovableHotbar>(movableHotbar)) {
            mod->lastAppliedPos = Vec2<float>{-120.f, -120.f};
            mod->update();
        }
    }

    if (auto movableScoreboard = ModuleManager::getModule("Movable Scoreboard"); movableScoreboard && movableScoreboard->isEnabled()) {
        if (std::shared_ptr<MovableScoreboard> mod = std::dynamic_pointer_cast<MovableScoreboard>(movableScoreboard)) {
            mod->lastAppliedPos = Vec2<float>{-120.f, -120.f};
            mod->update();
        }
    }

    if (auto movableBossbar = ModuleManager::getModule("Movable Bossbar"); movableBossbar && movableBossbar->isEnabled()) {
        if (std::shared_ptr<MovableBossbar> mod = std::dynamic_pointer_cast<MovableBossbar>(movableBossbar)) {
            mod->lastAppliedPos = Vec2<float>{-120.f, -120.f};
            mod->update();
        }
    }

    if (auto movableChat = ModuleManager::getModule("Movable Chat"); movableChat && movableChat->isEnabled()) {
        if (std::shared_ptr<MovableChat> mod = std::dynamic_pointer_cast<MovableChat>(movableChat)) {
            mod->lastAppliedPos = Vec2<float>{-120.f, -120.f};
            mod->update();
        }
    }

    if (auto movableCoords = ModuleManager::getModule("Movable Coordinates"); movableCoords && movableCoords->isEnabled()) {
        if (std::shared_ptr<MovableCoordinates> mod = std::dynamic_pointer_cast<MovableCoordinates>(movableCoords)) {
            mod->lastAppliedPos = Vec2<float>{-120.f, -120.f};
            mod->update();
        }
    }

    if (auto movableDayCounter = ModuleManager::getModule("Movable Day Counter"); movableDayCounter && movableDayCounter->isEnabled()) {
        if (std::shared_ptr<MovableDayCounter> mod = std::dynamic_pointer_cast<MovableDayCounter>(movableDayCounter)) {
            mod->lastAppliedPos = Vec2<float>{-120.f, -120.f};
            mod->update();
        }
    }

    if (delayDisable) {
        delayDisable = false;
        restored = true;
    }
}
