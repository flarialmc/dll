#pragma once

#include "../Module.hpp"
#include "../../Utils/WinrtUtils.hpp"

class MovableChat : public Module {
private:
    static inline std::string layerName = "chat_panel";
    Vec2<float> currentPos{};
    bool enabled = false;
    static inline Vec2<float> originalPos = Vec2<float>{0.0f, 0.0f};
    Vec2<float> currentSize = Vec2<float>{0.0f, 0.0f};
    Vec2<float> lastAppliedPos = Vec2<float>{0.0f, 0.0f};
    bool restored = false;
public:

    MovableChat() : Module("Chat Tweaks", "Makes the Minecraft chat better.", IDR_MOVABLE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        originalPos = Vec2<float>{0, 0};
        restored = false;
        Listen(this, SetupAndRenderEvent, &MovableChat::onSetupAndRender)
        Listen(this, RenderEvent, &MovableChat::onRender)
        Listen(this, UIControlGetPositionEvent, &MovableChat::onUIControlGetPosition)
        Listen(this, PacketEvent, &MovableChat::onPacket)

        if (FlarialGUI::inMenu) {
            FlarialGUI::Notify("To change the position of the " + name +", Please click " +
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
        Deafen(this, SetupAndRenderEvent, &MovableChat::onSetupAndRender)
        Deafen(this, RenderEvent, &MovableChat::onRender)
        Deafen(this, UIControlGetPositionEvent, &MovableChat::onUIControlGetPosition)
        Deafen(this, PacketEvent, &MovableChat::onPacket)

        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
        }
        if (settings.getSettingByName<float>("percentageY") == nullptr) {
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("here") == nullptr) settings.addSetting("here", true);
        if (settings.getSettingByName<bool>("pingsound") == nullptr) settings.addSetting("pingsound", true);
        if (this->settings.getSettingByName<std::string>("mode") == nullptr) settings.addSetting("mode", (std::string)"Xp Orb");
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, 5000, 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addToggle("Ping Sound", "Plays a sound when you're mentioned in the chat.", settings.getSettingByName<bool>("pingsound")->value);
        if (settings.getSettingByName<bool>("pingsound"))
        if (settings.getSettingByName<bool>("pingsound")->value)
        {
            this->addHeader("Ping Sound");
            this->addDropdown("Sound", "Choose which sound to play", std::vector<std::string>{"Xp Orb", "Custom"}, this->settings.getSettingByName<std::string>("mode")->value);
            
            this->addButton("Choose Sound", "Choose a custom sound", "Choose", [this] {
                WinrtUtils::pickAndCopyFiles(L"*", "\\assets");
            });
            this->addToggle("@here", "Plays the sound when you are mentioned via @here", settings.getSettingByName<bool>("here")->value);
        }
        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void onPacket(PacketEvent& event) {
        Packet* packet = event.getPacket();
        if (packet->getId() != MinecraftPacketIds::Text) return;

        auto pkt = reinterpret_cast<TextPacket*>(packet);
        std::string msg = pkt->message;
        if (!pkt || msg.empty() || pkt->type != TextPacketType::CHAT) return;


    }

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
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 26);

            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 26, width, height);


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