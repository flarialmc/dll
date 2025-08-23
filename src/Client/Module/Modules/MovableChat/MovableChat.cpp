#include "MovableChat.hpp"

#include "Modules/ClickGUI/ClickGUI.hpp"
#include "SDK/Client/Network/Packet/TextPacket.hpp"
#include "Utils/Render/PositionUtils.hpp"

MovableChat::MovableChat(): Module("Movable Chat", "Ability to move the chat.", IDR_MOVABLE_PNG, "") {
}

void MovableChat::onEnable() {
    restored = false;
    Listen(this, SetupAndRenderEvent, &MovableChat::onSetupAndRender)
    Listen(this, RenderEvent, &MovableChat::onRender)
    Listen(this, UIControlGetPositionEvent, &MovableChat::onUIControlGetPosition)
    Listen(this, PacketEvent, &MovableChat::onPacket)

    if (FlarialGUI::inMenu) {
        FlarialGUI::Notify("To change the position of the " + name + ", Please click " +
                           ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                               "editmenubind")->value + " in the settings tab.");
    }
    Module::onEnable();
}

void MovableChat::onDisable() {
    Module::onDisable();
    if (!restored) {
        delayDisable = true;
        return;
    }
    Deafen(this, SetupAndRenderEvent, &MovableChat::onSetupAndRender)
    Deafen(this, RenderEvent, &MovableChat::onRender)
    Deafen(this, UIControlGetPositionEvent, &MovableChat::onUIControlGetPosition)
    Deafen(this, PacketEvent, &MovableChat::onPacket)
}

void MovableChat::defaultConfig() {
    Module::defaultConfig("core");
    setDef("percentageX", 0.f);
    setDef("percentageY", 0.12658389f);
    setDef("here", true);
    setDef("pingsound", true);
    setDef("mode", (std::string) "Xp Orb");
}

void MovableChat::settingsRender(float settingsOffset) {
    initSettingsPage();

    this->addHeader("Movable Chat");
    this->addToggle("Ping Sound", "Plays a sound when you're mentioned in the chat.", "pingsound");
    if (settings.getSettingByName<bool>("pingsound"))
        if (getOps<bool>("pingsound")) {
            this->addHeader("Ping Sound");
            this->addDropdown("Sound", "Choose which sound to play", std::vector<std::string>{"Xp Orb", "Custom"}, "mode", true);

            this->addButton("Choose Sound", "Choose a custom sound", "Choose", [this] {
                WinrtUtils::pickAndCopyFiles(L"*", "\\assets");
            });
            this->addToggle("@here", "Plays the sound when you are mentioned via @here", "here");
        }
    FlarialGUI::UnsetScrollView();
    this->resetPadding();
}

void MovableChat::onPacket(PacketEvent &event) {
    if (!this->isEnabled() && !delayDisable) return;
    Packet *packet = event.getPacket();
    if (packet->getId() != MinecraftPacketIds::Text) return;

    auto pkt = reinterpret_cast<TextPacket *>(packet);
    std::string msg = pkt->message;
    if (!pkt || msg.empty() || pkt->type != TextPacketType::CHAT) return;
}

void MovableChat::onRender(RenderEvent &event) {
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
            FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 26, this->name);
            checkForRightClickAndOpenSettings(currentPos.x, currentPos.y, width, height);
        }

        if (currentPos.x != -120.0f) {
            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 26, width, height);

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

void MovableChat::onUIControlGetPosition(UIControlGetPositionEvent &event) {
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

void MovableChat::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled() && !delayDisable) return;
    update();
}

void MovableChat::update() {
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

void MovableChat::updatePosition(UIControl *control) {
    if (!(SDK::clientInstance && SDK::clientInstance->getLocalPlayer())) return;

    if ((enabledState && originalPos == Vec2<float>{0, 0}) || delayDisable) {
        auto guiData = SDK::clientInstance->getGuiData();
        auto scaledSize = guiData->ScreenSizeScaled;
        auto recalculatedPos = Vec2<float>{0, 62 + 12};
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
