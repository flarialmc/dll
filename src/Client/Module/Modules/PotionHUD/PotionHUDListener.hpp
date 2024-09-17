#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/Client/Render/ItemRenderer.hpp"
#include "../../../../Utils/Render/PositionUtils.hpp"
#include "../ClickGUI/ClickGUIRenderer.hpp"
#include <Windows.h>
#include <chrono>

class PotionHUDListener : public Listener {
public:

    Module *module;
    Vec2<float> currentPos;
    bool enabled = false;

    float testSpacing = 10;

    void renderText() const {
        if(FlarialGUI::inMenu && !ClickGUIRenderer::editmenu) return;

        auto rgb1 = module->settings.getSettingByName<bool>("colorLow_rgb")->value;
        auto rgb2 = module->settings.getSettingByName<bool>("colorMain_rgb")->value;

        auto rgbColor = FlarialGUI::rgbColor;

        auto lowColor = rgb1 ? rgbColor : FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("colorLow")->value);
        auto mainColor = rgb2 ? rgbColor : FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("colorMain")->value);

        int warning_seconds = 5;

        const float textWidth = Constraints::RelativeConstraint(module->settings.getSettingByName<float>("textSize")->value, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr)
                if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {

                    float spacing = testSpacing;

                    auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{spacing, spacing},
                                                                             module->settings.getSettingByName<float>(
                                                                                     "uiscale")->value);
                    spacing = scaledPos.y;

                    float xmodifier = 0.0f;
                    float ymodifier = 0.0f;

                    float xoffset = 0.0f;
                    float yoffset = 0.0f;

                    auto effects = SDK::clientInstance->getLocalPlayer()->getMobEffectsComponent()->effects;
                    for(const auto& effect : effects) {
                        if(!effect.isValid()) continue;
                        std::string text = effect.getName();
                        std::wstring widestr = std::wstring(text.begin(), text.end());
                        const wchar_t *widecstr = widestr.c_str();
                        FlarialGUI::FlarialTextWithFont(
                                currentPos.x + xmodifier + xoffset,
                                currentPos.y + ymodifier + yoffset, widecstr, textWidth * 6.9f,
                                textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                textWidth,
                                DWRITE_FONT_WEIGHT_NORMAL, effect.duration / 20 <= warning_seconds ? lowColor : mainColor,true);
                        ymodifier += spacing;
                    }
                }
        }
    }

    void onRender(RenderEvent &event) override {
        if (ClientInstance::getTopScreenName() == "hud_screen" &&
            module->isEnabled()) {

            float s = Constraints::RelativeConstraint(0.04, "height", true) *
                      module->settings.getSettingByName<float>("uiscale")->value;

            float spacing = Constraints::RelativeConstraint(0.0135, "height", true) *
                            module->settings.getSettingByName<float>("uiscale")->value;

            Vec2<float> settingperc = Vec2<float>(module->settings.getSettingByName<float>("percentageX")->value,
                                                  module->settings.getSettingByName<float>("percentageY")->value);


            float width = s * 3 + spacing * 3;
            float height = s;

            if (settingperc.x != 0)
                currentPos = Vec2<float>(settingperc.x * MC::windowSize.x,
                                         settingperc.y * MC::windowSize.y);
            else
                currentPos = Constraints::CenterConstraint(width, height);

            if (ClickGUIRenderer::editmenu) {
                // bounding boxes
                FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 23);
            }

            Vec2<float> vec2;
            // bounding boxes
            vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 23, width, height);

            currentPos.x = vec2.x;
            currentPos.y = vec2.y;

            Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y);

            module->settings.setValue("percentageX", percentages.x);
            module->settings.setValue("percentageY", percentages.y);

            if (ClickGUIRenderer::editmenu) {
                FlarialGUI::RoundedRect(currentPos.x, currentPos.y, D2D1::ColorF(D2D1::ColorF::White, 0.4f), width, height);
                FlarialGUI::UnsetWindowRect();
            }

            renderText();
        }

        if (SDK::getCurrentScreen() != "hud_screen") ClickGUIRenderer::editmenu = false;
    }

    void onSetupAndRender(SetupAndRenderEvent &event) override {
        if(this->module->isEnabled())
        if (ClientInstance::getTopScreenName() == "hud_screen") {
            // TODO: Make icons work
//            Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
//
//            if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
//                float spacing = 15 * module->settings.getSettingByName<float>("uiscale")->value;
//
//                float xmodifier = 0.0f;
//                float ymodifier = 0.0f;
//
//                auto muirc = event.getMuirc();
//                auto effects = SDK::clientInstance->getLocalPlayer()->getMobEffectsComponent()->effects;
//                for(const auto& effect : effects) {
//                    if(!effect.isValid()) continue;
//                    auto location = effect.getTextureLocation();
//                    auto texture = muirc->createTexture(location, false);
//                    //auto position = Vec2<float>(scaledPos.x + xmodifier, scaledPos.y + ymodifier);
//                    auto position = Vec2<float>(50, 50);
//                    static auto size = Vec2<float>(18.0f, 18.0f);
//                    static auto uvPos = Vec2<float>(0.f, 0.f);
//                    static auto uvSize = Vec2<float>(1.0f, 1.0f);
//                    muirc->drawImage(texture, position, size,uvPos,uvSize);
//                    ymodifier += spacing;
//                }
//            }
        }
    }

public:
    explicit PotionHUDListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
        this->currentPos = Vec2<float>(0, 0);
    }
};
