#include "../../../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUIRenderer.hpp"
#include "../../../../../../Assets/Assets.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_text FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_text")->value)
#define o_colors_text clickgui->settings.getSettingByName<float>("o_colors_text")->value
#define colors_text_rgb clickgui->settings.getSettingByName<bool>("colors_text_rgb")->value

#define colors_primary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary1")->value)
#define o_colors_primary1 clickgui->settings.getSettingByName<float>("o_colors_primary1")->value
#define colors_primary1_rgb clickgui->settings.getSettingByName<bool>("colors_primary1_rgb")->value

#define colors_secondary4 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary4")->value)
#define o_colors_secondary4 clickgui->settings.getSettingByName<float>("o_colors_secondary4")->value
#define colors_secondary4_rgb clickgui->settings.getSettingByName<bool>("colors_secondary4_rgb")->value

#define colors_enabled FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_enabled")->value)
#define o_colors_enabled clickgui->settings.getSettingByName<float>("o_colors_enabled")->value
#define colors_enabled_rgb clickgui->settings.getSettingByName<bool>("colors_enabled_rgb")->value

#define colors_disabled FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_disabled")->value)
#define o_colors_disabled clickgui->settings.getSettingByName<float>("o_colors_disabled")->value
#define colors_disabled_rgb clickgui->settings.getSettingByName<bool>("colors_disabled_rgb")->value

#define colors_mod1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod1")->value)
#define o_colors_mod1 clickgui->settings.getSettingByName<float>("o_colors_mod1")->value
#define colors_mod1_rgb clickgui->settings.getSettingByName<bool>("colors_mod1_rgb")->value

#define colors_mod2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod2")->value)
#define o_colors_mod2 clickgui->settings.getSettingByName<float>("o_colors_mod2")->value
#define colors_mod2_rgb clickgui->settings.getSettingByName<bool>("colors_mod2_rgb")->value

#define colors_mod3 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod3")->value)
#define o_colors_mod3 clickgui->settings.getSettingByName<float>("o_colors_mod3")->value
#define colors_mod3_rgb clickgui->settings.getSettingByName<bool>("colors_mod3_rgb")->value

#define colors_mod4 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_mod4")->value)
#define o_colors_mod4 clickgui->settings.getSettingByName<float>("o_colors_mod4")->value
#define colors_mod4_rgb clickgui->settings.getSettingByName<bool>("colors_mod4_rgb")->value

std::map<int, ID2D1Bitmap *> ClickGUIElements::images;
std::vector<Vec2<float>> sizes;
std::vector<Vec2<float>> shadowSizes;

void ClickGUIElements::ModCard(float x, float y, Module *mod, int iconId, const int index, bool visible) {
    Vec2<float> round = Constraints::RoundingConstraint(34, 34);


    if (index > sizes.size() - 1 || index == 0) {
        float nigga = Constraints::RelativeConstraint(0.19f, "height", true);
        float gaynigga = Constraints::RelativeConstraint(0.141f, "height", true);

        sizes.emplace_back(nigga, gaynigga);
    }

    if (index > shadowSizes.size() - 1 || index == 0) {
        shadowSizes.emplace_back(0.01, 0.01);
    }

    if (!visible)
        return;

    // Bottom rounded rect
    float BottomRoundedWidth = sizes[index].x;
    float BottomRoundedHeight = sizes[index].y;

    float realY = y;
    float diffX;
    float diffY;

    if (FlarialGUI::isInScrollView) realY += FlarialGUI::scrollpos;

    if (FlarialGUI::CursorInRect(x, realY, BottomRoundedWidth, BottomRoundedHeight)) {

        FlarialGUI::lerp(sizes[index].x, Constraints::RelativeConstraint(0.186f, "height", true),
                         0.15f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(sizes[index].y, Constraints::RelativeConstraint(0.149f, "height", true),
                         0.15f * FlarialGUI::frameFactor);

        diffX = (sizes[index].x - Constraints::RelativeConstraint(0.178f, "height", true)) / 2.0f;
        diffY = (sizes[index].y - Constraints::RelativeConstraint(0.141f, "height", true)) / 2.0f;

        FlarialGUI::lerp(shadowSizes[index].x, BottomRoundedWidth, 0.25f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(shadowSizes[index].y, BottomRoundedHeight, 0.25f * FlarialGUI::frameFactor);

    } else {

        FlarialGUI::lerp(sizes[index].x, Constraints::RelativeConstraint(0.178f, "height", true),
                         0.15f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(sizes[index].y, Constraints::RelativeConstraint(0.141f, "height", true),
                         0.15f * FlarialGUI::frameFactor);

        FlarialGUI::lerp(shadowSizes[index].x, 0.01f, 0.01f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(shadowSizes[index].y, 0.01f, 0.01f * FlarialGUI::frameFactor);

        diffX = (sizes[index].x - Constraints::RelativeConstraint(0.178f, "height", true)) / 2.0f;
        diffY = (sizes[index].y - Constraints::RelativeConstraint(0.141f, "height", true)) / 2.0f;

    }

    x -= diffX;
    y -= diffY;

    if (shadowSizes[index].x > Constraints::RelativeConstraint(0.255f, "height")) {


        float diffX2 = (shadowSizes[index].x - BottomRoundedWidth) / 2.0f;
        float diffY2 = (shadowSizes[index].y - BottomRoundedHeight) / 2.0f;

        FlarialGUI::ShadowRect(
            Vec2<float>(x, y),
            Vec2<float>(BottomRoundedWidth, BottomRoundedHeight),
            D2D1::ColorF(0, 0, 0, .7),
            round.x,
            (diffX*5)
        );
    }

    D2D1_COLOR_F mod1Col = colors_mod1_rgb ? FlarialGUI::rgbColor : colors_mod1;
    mod1Col.a = o_colors_mod1;

    D2D1_COLOR_F mod2Col = colors_mod2_rgb ? FlarialGUI::rgbColor : colors_mod2;
    mod2Col.a = o_colors_mod2;

    FlarialGUI::RoundedRect(x, y, mod1Col, BottomRoundedWidth, BottomRoundedHeight, round.x, round.x);

    // Top rounded rect

    float TopRoundedHeight = Constraints::SpacingConstraint(sizes[index].y, 0.635f);

    FlarialGUI::PushSize(x, y, BottomRoundedWidth, BottomRoundedHeight);
    // Mod Name
    float textx = Constraints::PercentageConstraint(0.119, "left");
    float texty = Constraints::PercentageConstraint(0.415, "top");
    float textWidth = Constraints::RelativeConstraint(1.0);
    float textHeight = Constraints::RelativeConstraint(0.2);

    FlarialGUI::FlarialTextWithFont(textx, texty, FlarialGUI::to_wide(mod->name).c_str(), textWidth, textHeight,
                                    DWRITE_TEXT_ALIGNMENT_CENTER,
                                    Constraints::SpacingConstraint(0.854, textWidth),
                                    DWRITE_FONT_WEIGHT_NORMAL, FlarialGUI::HexToColorF("8b767a"),false);

    // Mod icon

    D2D1_COLOR_F mod3Col = colors_mod3_rgb ? FlarialGUI::rgbColor : colors_mod3;
    mod3Col.a = o_colors_mod3;

    float modiconx = Constraints::PercentageConstraint(0.40, "left");
    float modicony = Constraints::PercentageConstraint(0.11, "top");

    float paddingSize = Constraints::RelativeConstraint(0.28);

    FlarialGUI::Tooltip("mod_" + std::to_string(index), x, realY, mod->description, BottomRoundedWidth,
                        TopRoundedHeight);

    FlarialGUI::RoundedRect(modiconx, modicony, mod3Col,
                            paddingSize, paddingSize, 7.5, 7.5);

    paddingSize = Constraints::RelativeConstraint(0.20);
    modiconx = Constraints::PercentageConstraint(0.43, "left");
    modicony = Constraints::PercentageConstraint(0.15, "top");

    // enabled / disabled button DECLARATIONS

    std::string text;
    text = mod->isEnabled() ? "Enabled" : "Disabled";
    D2D1_COLOR_F enabledColor = colors_enabled_rgb ? FlarialGUI::rgbColor : colors_enabled;
    D2D1_COLOR_F disabledColor = colors_disabled_rgb ? FlarialGUI::rgbColor : colors_disabled;

    enabledColor.a = o_colors_enabled;
    disabledColor.a = o_colors_disabled;



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

    D2D1_COLOR_F mod4Col = colors_mod4_rgb ? FlarialGUI::rgbColor : colors_mod4;
    mod4Col.a = o_colors_mod4;

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

    FlarialGUI::image(iconId, D2D1::RectF(modiconx, modicony, modiconx + paddingSize, modicony + paddingSize));

    // actually button

    if (FlarialGUI::buttonColors.size() >= index)
        FlarialGUI::buttonColors[index] = FlarialGUI::LerpColor(FlarialGUI::buttonColors[index], to,
                                                                0.15f * FlarialGUI::frameFactor);
    else FlarialGUI::buttonColors.resize(index);

    round = Constraints::RoundingConstraint(22, 22);

    D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
    textCol.a = o_colors_text;

    D2D1_COLOR_F buttonColor;
    buttonColor = D2D1::ColorF(FlarialGUI::buttonColors[index].r - FlarialGUI::darkenAmounts[index], FlarialGUI::buttonColors[index].g - FlarialGUI::darkenAmounts[index],
                                   FlarialGUI::buttonColors[index].b - FlarialGUI::darkenAmounts[index], FlarialGUI::buttonColors[index].a);

    FlarialGUI::RoundedRect((buttonx - buttonWidth) - Constraints::SpacingConstraint(0.105f, paddingwidth), buttony - buttonHeight,
                                  buttonColor, buttonWidth - Constraints::SpacingConstraint(1.5, paddingwidth), buttonHeight, 0.f, 0.f);

    if (FlarialGUI::RoundedButton(index, buttonx - buttonWidth, buttony - buttonHeight,
                                  FlarialGUI::buttonColors[index], textCol,
                                  L"", buttonWidth, buttonHeight, round.x,
                                  round.x)) {
        mod->toggle();
                                  }

    FlarialGUI::FlarialTextWithFont((buttonx - buttonWidth) - Constraints::SpacingConstraint(0.15f, paddingwidth), buttony - buttonHeight, FlarialGUI::to_wide(text).c_str(), buttonWidth, buttonHeight, DWRITE_TEXT_ALIGNMENT_CENTER, buttonWidth * 1.08, DWRITE_FONT_WEIGHT_NORMAL, false);

    if (FlarialGUI::isInScrollView)
        buttony += FlarialGUI::scrollpos;

    if (FlarialGUI::CursorInRect(settingx, buttony - buttonHeight,
                                 paddingwidth + Constraints::RelativeConstraint(0.26), buttonHeight) && MC::mouseButton == MouseButton::Left &&
        !MC::held) {
        FlarialGUI::TextBoxes[0].isActive = false;
        MC::mouseButton = MouseButton::None;
        ClickGUIRenderer::page.type = "settings";
        ClickGUIRenderer::page.module = mod->name;
        FlarialGUI::TextBoxes[0].isActive = false;
        FlarialGUI::scrollpos = 0;
        FlarialGUI::barscrollpos = 0;
        GUIMouseListener::accumilatedPos = 0;
        GUIMouseListener::accumilatedBarPos = 0;
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

        FlarialGUI::ImRotateStart();
        FlarialGUI::image(IDR_SETTINGS_WHITE_PNG, imagerectf, "PNG", false);
        FlarialGUI::ImRotateEnd(rotationAngle, rotationCenter);
    }
}