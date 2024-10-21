#pragma once

#include "../Module.hpp"
#include "../ClickGUI/ClickGUI.hpp"
#include "../../../Events/EventManager.hpp"

class MovableChat : public Module {
private:
    Vec2<float> currentPos{0.0f, 0.0f};
    bool enabled = true;
    static inline Vec2<float> oriXy = Vec2<float>{0.0f, 0.0f};
public:

    MovableChat() : Module("Movable Chat", "Makes the Minecraft chat movable.", IDR_MAN_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        Listen(this, RenderEvent, &MovableChat::onRender)
        Listen(this, SetupAndRenderEvent, &MovableChat::onSetupAndRender)
        Module::onEnable();
    }

    void onDisable() override {
        Listen(this, RenderEvent, &MovableChat::onRender)
        Listen(this, SetupAndRenderEvent, &MovableChat::onSetupAndRender)
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 21.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
        }
        if (settings.getSettingByName<float>("percentageY") == nullptr) {
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);

    }

    void settingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, 40, 2);
        FlarialGUI::SetScrollView(toggleX, toggleY, Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(3, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
                                           toggleY,
                                           this->settings.getSettingByName<float>("uiscale")->value, 40.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;

        FlarialGUI::UnsetScrollView();
    }

    void onRender(RenderEvent &event) {

        if (!this->isEnabled()) enabled = false;

        if (ClientInstance::getTopScreenName() == "hud_screen" &&
            this->isEnabled() ||
            ClientInstance::getTopScreenName() == "pause_screen" &&
            this->isEnabled()) {

            if (!enabled && FlarialGUI::inMenu) {
                FlarialGUI::Notify("To change the position of the chat, Please click " +
                                   ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                           "editmenubind")->value);
                enabled = true;
            }

            float width = Constraints::RelativeConstraint(0.0366, "height", true) *
                          this->settings.getSettingByName<float>("uiscale")->value;
            float height = Constraints::RelativeConstraint(0.015, "height", true) *
                           this->settings.getSettingByName<float>("uiscale")->value;


            Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
                                                  this->settings.getSettingByName<float>("percentageY")->value);

            if (settingperc.x != 0)
                currentPos = Vec2<float>(settingperc.x * MC::windowSize.x,
                                         settingperc.y * MC::windowSize.y);
            else if (settingperc.x == 0 and oriXy.x != 0.0f)
                currentPos = Vec2<float>{oriXy.x, oriXy.y};

            if (ClickGUI::editmenu)
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 21);

            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 21, width, height);


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
        } else {
            enabled = false;
        }
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        if (this->isEnabled()) {

            if (SDK::screenView->VisualTree->root->getLayerName() == "hud_screen") {

                SDK::screenView->VisualTree->root->forEachControl([this](const std::shared_ptr<UIControl> &control) {

                    if (control->getLayerName() == "chat_panel" && this->isEnabled()) {


                        if (oriXy.x == 0.0f) {
                            oriXy.x = control->x;
                            oriXy.y = control->y;
                        }

                        Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

                        control->x = scaledPos.x + 5;
                        control->y = scaledPos.y;

                        control->scale = this->settings.getSettingByName<float>("uiscale")->value + 100;
                    }
                });
            }
        }
    }
};