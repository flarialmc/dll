#include "../../../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../../../../Assets/Assets.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include "../Utils/WinrtUtils.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

std::map<int, ID2D1Bitmap *> ClickGUIElements::images;
std::vector<Vec2<float>> sizesr;
std::vector<Vec2<float>> shadowsizesr;
std::vector<bool> hoveringFav;

using namespace winrt::Windows::UI::Core;
void ClickGUIElements::ModCard(float x, float y, Module *mod, int iconId, const int index, bool visible, float opacity) {
    if (opacity == -600.f) opacity = ClickGUI::modcardOpacity;
    Vec2<float> round = Constraints::RoundingConstraint(34, 34);

    if (index > sizesr.size() - 1 || index == 0) {
        float h1 = Constraints::RelativeConstraint(0.19f, "height", true);
        float h2 = Constraints::RelativeConstraint(0.141f, "height", true);

        sizesr.emplace_back(h1, h2);
    }

    if (index > shadowsizesr.size() - 1 || index == 0) {
        shadowsizesr.emplace_back(0.01, 0.01);
    }

    if (index > hoveringFav.size() - 1 || index == 0) {
        hoveringFav.emplace_back(false);
    }

    if (!visible) return;

    // Bottom rounded rect
    float BottomRoundedWidth = sizesr[index].x;
    float BottomRoundedHeight = sizesr[index].y;

    float realY = y;
    float diffX;
    float diffY;

    if (FlarialGUI::isInScrollView) realY += FlarialGUI::scrollpos;

    if (FlarialGUI::CursorInRect(x, realY, BottomRoundedWidth, BottomRoundedHeight)) {

        FlarialGUI::lerp(sizesr[index].x, Constraints::RelativeConstraint(0.186f, "height", true),
                         0.15f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(sizesr[index].y, Constraints::RelativeConstraint(0.149f, "height", true),
                         0.15f * FlarialGUI::frameFactor);

        diffX = (sizesr[index].x - Constraints::RelativeConstraint(0.178f, "height", true)) / 2.0f;
        diffY = (sizesr[index].y - Constraints::RelativeConstraint(0.141f, "height", true)) / 2.0f;

        FlarialGUI::lerp(shadowsizesr[index].x, BottomRoundedWidth, 0.25f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(shadowsizesr[index].y, BottomRoundedHeight, 0.25f * FlarialGUI::frameFactor);

    } else {

        FlarialGUI::lerp(sizesr[index].x, Constraints::RelativeConstraint(0.178f, "height", true),
                         0.15f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(sizesr[index].y, Constraints::RelativeConstraint(0.141f, "height", true),
                         0.15f * FlarialGUI::frameFactor);

        FlarialGUI::lerp(shadowsizesr[index].x, 0.01f, 0.01f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(shadowsizesr[index].y, 0.01f, 0.01f * FlarialGUI::frameFactor);

        diffX = (sizesr[index].x - Constraints::RelativeConstraint(0.178f, "height", true)) / 2.0f;
        diffY = (sizesr[index].y - Constraints::RelativeConstraint(0.141f, "height", true)) / 2.0f;

    }

    x -= diffX;
    y -= diffY;

    if (shadowsizesr[index].x > Constraints::RelativeConstraint(0.255f, "height")) {


        float diffX2 = (shadowsizesr[index].x - BottomRoundedWidth) / 2.0f;
        float diffY2 = (shadowsizesr[index].y - BottomRoundedHeight) / 2.0f;

        FlarialGUI::ShadowRect(
            Vec2<float>(x, y),
            Vec2<float>(BottomRoundedWidth, BottomRoundedHeight),
            D2D1::ColorF(0, 0, 0, .7),
            round.x,
            (diffX*5)
        );
    }

    D2D1_COLOR_F mod1Col = ClickGUI::getColor("modcard1");
    D2D1_COLOR_F mod2Col = ClickGUI::getColor("modcard2");

    FlarialGUI::RoundedRect(x, y, mod1Col, BottomRoundedWidth, BottomRoundedHeight, round.x, round.x);
    mod->checkForRightClickAndOpenSettings(x, realY, BottomRoundedWidth, BottomRoundedHeight);


    // Top rounded rect

    float TopRoundedHeight = Constraints::SpacingConstraint(sizesr[index].y, 0.635f);

    FlarialGUI::PushSize(x, y, BottomRoundedWidth, BottomRoundedHeight);

    // Mod Name
    float textx = Constraints::PercentageConstraint(0.119, "left");
    float texty = Constraints::PercentageConstraint(0.415, "top");
    float textWidth = Constraints::RelativeConstraint(1.0);
    float textHeight = Constraints::RelativeConstraint(0.2);
    D2D1_COLOR_F textCol = FlarialGUI::HexToColorF("8b767a");
    textCol.a = clickgui->getColor("globalText", "ClickGUI").a;



    FlarialGUI::FlarialTextWithFont(textx, texty, FlarialGUI::to_wide(mod->name).c_str(), textWidth, textHeight,
                                    DWRITE_TEXT_ALIGNMENT_CENTER,
                                    Constraints::SpacingConstraint(0.854, textWidth),
                                    DWRITE_FONT_WEIGHT_NORMAL, textCol,false);

    // Mod icon

    D2D1_COLOR_F mod3Col = ClickGUI::getColor("modcard3");

    float modiconx = Constraints::PercentageConstraint(0.40, "left");
    float modicony = Constraints::PercentageConstraint(0.11, "top");

    float paddingSize = Constraints::RelativeConstraint(0.28);
    if (!FlarialGUI::CursorInRect(modiconx, modicony + FlarialGUI::scrollpos, paddingSize, paddingSize) && !FlarialGUI::CursorInRect(Constraints::PercentageConstraint(0.43, "left"), Constraints::PercentageConstraint(0.15, "top") + FlarialGUI::scrollpos, paddingSize, paddingSize)) {
        FlarialGUI::Tooltip("mod_" + FlarialGUI::cached_to_string(index), x, realY, mod->description, BottomRoundedWidth, TopRoundedHeight, true, false, std::chrono::milliseconds(3000));
    }

    FlarialGUI::RoundedRect(modiconx, modicony, mod3Col,
                            paddingSize, paddingSize, 7.5, 7.5);

    paddingSize = Constraints::RelativeConstraint(0.20);
    modiconx = Constraints::PercentageConstraint(0.43, "left");
    modicony = Constraints::PercentageConstraint(0.15, "top");

    // enabled / disabled button DECLARATIONS

    std::string text;
    text = mod->isEnabled() ? "Enabled" : "Disabled";
    D2D1_COLOR_F enabledColor = ClickGUI::getColor("modCardEnabled");
    D2D1_COLOR_F disabledColor = ClickGUI::getColor("modCardDisabled");

    float buttonWidth = Constraints::RelativeConstraint(0.68, "width");
    float buttonHeight = Constraints::RelativeConstraint(0.259);

    float buttonx = Constraints::PercentageConstraint(0.04, "right");
    float buttony = Constraints::PercentageConstraint(0.093, "bottom");

    D2D1_COLOR_F to = text == "Enabled" ? enabledColor : disabledColor;

    // en/dis button end

    // Settings Button
    float settingswidth = Constraints::RelativeConstraint(0.17);
    float iconwidth = Constraints::RelativeConstraint(0.15);
    float settingsheightspac = Constraints::SpacingConstraint(0.31, settingswidth);

    float paddingwidth = Constraints::RelativeConstraint(0.26);
    float paddingspacing = Constraints::SpacingConstraint(4.84, paddingwidth);
    float paddingheightspac = Constraints::SpacingConstraint(0.05, paddingwidth);

    round = Constraints::RoundingConstraint(20, 20);

    float settingx = Constraints::PercentageConstraint(0.0485, "left");
    float settingx2 = Constraints::PercentageConstraint(0.0885, "left");

    FlarialGUI::RoundedRect(settingx, buttony - buttonHeight,
                            mod3Col, paddingwidth + Constraints::RelativeConstraint(0.26),
                            buttonHeight, round.x, round.x);

    D2D1_COLOR_F mod4Col = ClickGUI::getColor("modcard4");


    if (!Client::settings.getSettingByName<bool>("noicons")->value) {
        RotatingGear(index, settingx2, (buttony - buttonHeight) + Constraints::SpacingConstraint(0.162, paddingwidth), settingswidth, settingswidth, iconwidth, iconwidth);
    }

    // settings button end
    /*
    if (iconId != -1 && images[iconId] == nullptr) {
        FlarialGUI::LoadImageFromResource(iconId, &images[iconId]);

    } else if (images[iconId] != nullptr) {

        if (FlarialGUI::isInScrollView) {
            modicony += FlarialGUI::scrollpos;
        }

        if (!Client::settings.getSettingByName<bool>("noicons")->value &&
            FlarialGUI::isRectInRect(FlarialGUI::ScrollViewRect,
                                     D2D1::RectF(modiconx, modicony, modiconx + paddingSize, modicony + paddingSize)))
            D2D::context->DrawBitmap(images[iconId], D2D1::RectF(modiconx, modicony, modiconx + paddingSize,
                                                                    modicony + paddingSize));
    }
    */
    D2D1_COLOR_F modicon = ClickGUI::getColor("modicon");

    if (mod->settings.getSettingByName<bool>("favorite")->value || FlarialGUI::CursorInRect(modiconx, modicony + FlarialGUI::scrollpos, paddingSize, paddingSize)) {
        if (mod->settings.getSettingByName<bool>("favorite")->value/* && !FlarialGUI::CursorInRect(modiconx, modicony + FlarialGUI::scrollpos, paddingSize, paddingSize)*/) {
            modicon = D2D1::ColorF(D2D1::ColorF::Gold);
        }
        if (FlarialGUI::CursorInRect(modiconx, modicony + FlarialGUI::scrollpos, paddingSize, paddingSize)) {
            FlarialGUI::Tooltip("favorite_" + FlarialGUI::cached_to_string(index), x, realY, mod->settings.getSettingByName<bool>("favorite")->value ? "Unfavorite?" : "Favorite?", BottomRoundedWidth, TopRoundedHeight, true, false, std::chrono::milliseconds(1));
        }
    }
    if (FlarialGUI::CursorInRect(modiconx, modicony + FlarialGUI::scrollpos, paddingSize, paddingSize)) {
        if (!hoveringFav[index]) {
            WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Hand);
            hoveringFav[index] = true;
        }
    } else if (!FlarialGUI::CursorInRect(modiconx, modicony + FlarialGUI::scrollpos, paddingSize, paddingSize)) {
        if (hoveringFav[index]) {
            WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
            hoveringFav[index] = false;
        }
    }
    FlarialGUI::image(iconId, D2D1::RectF(modiconx, modicony, modiconx + paddingSize, modicony + paddingSize), "PNG", true, FlarialGUI::D2DColorToImColor(modicon)); //, FlarialGUI::D2DColorToImColor(modicon)

    // actually button

    if (FlarialGUI::buttonColors.size() >= index)
        FlarialGUI::buttonColors[index] = FlarialGUI::LerpColor(FlarialGUI::buttonColors[index], to,
                                                                0.15f * FlarialGUI::frameFactor);
    else FlarialGUI::buttonColors.resize(index);

    round = Constraints::RoundingConstraint(22, 22);

    D2D1_COLOR_F textCol2 = ClickGUI::getColor("globalText");


    D2D1_COLOR_F buttonColor;
    buttonColor = D2D1::ColorF(FlarialGUI::buttonColors[index].r - FlarialGUI::darkenAmounts[index], FlarialGUI::buttonColors[index].g - FlarialGUI::darkenAmounts[index],
                                   FlarialGUI::buttonColors[index].b - FlarialGUI::darkenAmounts[index], FlarialGUI::buttonColors[index].a);




    FlarialGUI::RoundedRect((buttonx - buttonWidth) - Constraints::SpacingConstraint(0.105f, paddingwidth), buttony - buttonHeight,
                                  buttonColor, buttonWidth - Constraints::SpacingConstraint(1.5, paddingwidth), buttonHeight, 0.f, 0.f);

    if (FlarialGUI::RoundedButton(index, buttonx - buttonWidth, buttony - buttonHeight,
                                  FlarialGUI::buttonColors[index], textCol2,
                                  L"", buttonWidth, buttonHeight, round.x,
                                  round.x)) {
        if (ClickGUI::baseHeightActual > 0.6f) {
            if (mod->isScriptingModule) {
                bool newState = !mod->settings.getSettingByName<bool>("enabled")->value;
                if (newState) {
                    mod->onEnable();
                    mod->settings.getSettingByName<bool>("enabled")->value = newState;
                }
                else {
                    mod->onDisable();
                    mod->settings.getSettingByName<bool>("enabled")->value = newState;
                }
            }
            mod->toggle();
        }
    }

    FlarialGUI::FlarialTextWithFont((buttonx - buttonWidth) - Constraints::SpacingConstraint(0.15f, paddingwidth), buttony - buttonHeight, FlarialGUI::to_wide(text).c_str(), buttonWidth, buttonHeight, DWRITE_TEXT_ALIGNMENT_CENTER, buttonWidth * 1.08, DWRITE_FONT_WEIGHT_NORMAL, textCol2, false);

    D2D1::RectF();
    if (FlarialGUI::isInScrollView)
        buttony += FlarialGUI::scrollpos;


    // release
    static bool fix;
    if (MC::mouseAction == Release) fix = false;
    if (FlarialGUI::CursorInRect(modiconx, modicony + FlarialGUI::scrollpos, paddingSize, paddingSize) && MC::mouseButton == Left && MC::mouseAction == Press && !fix) {
        fix = true;
        mod->settings.getSettingByName<bool>("favorite")->value = !mod->settings.getSettingByName<bool>("favorite")->value;
            FlarialGUI::Notify("Reopen this menu to view changes.");
            ClickGUI::favoriteStart = std::chrono::high_resolution_clock::now();
      }

    if (FlarialGUI::CursorInRect(settingx, buttony - buttonHeight,
                                 paddingwidth + Constraints::RelativeConstraint(0.26), buttonHeight) && MC::mouseButton == MouseButton::Left &&
        !MC::held) {
        FlarialGUI::TextBoxes[0].isActive = false;
        MC::mouseButton = MouseButton::None;
        ClickGUI::page.type = "settings";
        ClickGUI::page.module = mod->name;
        ClickGUI::curr = "settings";
        FlarialGUI::scrollpos = 0;
        FlarialGUI::barscrollpos = 0;
        ClickGUI::accumilatedPos = 0;
        ClickGUI::accumilatedBarPos = 0;
    }
    FlarialGUI::PopSize();
}

#include "../../Structs/ImagesClass.hpp"

void ClickGUIElements::RotatingGear(int index, float x, float y, float width, float height, float imageWidth, float imageHeight) {

    float imageY = y;

    if (FlarialGUI::isInScrollView) {
        y += FlarialGUI::scrollpos;
        imageY += FlarialGUI::scrollpos;
    }

    x = x + (width - imageWidth) / 2.0f;
    imageY = imageY + (height - imageHeight) / 2.0f;

    D2D1_RECT_F imagerectf = D2D1::RectF(x, imageY, x + imageWidth, imageY + imageHeight);

    if (ImagesClass::images[IDR_SETTINGS_WHITE_PNG] == nullptr) {
        FlarialGUI::LoadImageFromResource(IDR_SETTINGS_WHITE_PNG, &ImagesClass::images[IDR_SETTINGS_WHITE_PNG]);

    } else if (ImagesClass::images[IDR_SETTINGS_WHITE_PNG] != nullptr) {

        if (FlarialGUI::CursorInRect(x, y, width, height)) {
            FlarialGUI::lerp(FlarialGUI::rotationAngles[index], FlarialGUI::rotationAngles[index] + 15, 0.24f * FlarialGUI::frameFactor);
        }

        float rotationAngle = FlarialGUI::rotationAngles[index];
        ImVec2 rotationCenter(x + imageWidth / 2.0f, imageY + imageHeight / 2.0f);

        D2D1_COLOR_F settingicon = ClickGUI::getColor("modsettings");

        FlarialGUI::ImRotateStart();
        FlarialGUI::image(IDR_SETTINGS_WHITE_PNG, imagerectf, "PNG", false, FlarialGUI::D2DColorToImColor(settingicon));
        FlarialGUI::ImRotateEnd(rotationAngle, rotationCenter);
    }
}