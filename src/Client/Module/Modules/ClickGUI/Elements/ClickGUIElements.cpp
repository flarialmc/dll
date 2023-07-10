#pragma once
#include "../ClickGUIRenderer.hpp"
#include "ClickGUIElements.hpp"


std::map<std::string, ID2D1Bitmap*> ClickGUIElements::images;
std::vector<D2D1_MATRIX_3X2_F> ClickGUIElements::matrixes;
std::vector<Vec2<float>> sizes;

void ClickGUIElements::ModCard(float x, float y, Module* mod, const std::string iconpath, const int index)
{

    Vec2<float> vec = FlarialGUI::CalculateMovedXY(x, y, 0);
    x = vec.x;
    y = vec.y;

    Vec2<float> round = Constraints::RoundingConstraint(34, 34);


    if(index > sizes.size() - 1 || index == 0) {
        float nigga = Constraints::RelativeConstraint(0.19f, "height", true);
        float gaynigga = Constraints::RelativeConstraint(0.141f, "height", true);

        sizes.emplace_back(nigga, gaynigga);
    }

    // Bottom rounded rect
    float BottomRoundedWidth = sizes[index].x;
    float BottomRoundedHeight = sizes[index].y;

    float realY = y;
    float diffX = 0;
    float diffY = 0;

    if(FlarialGUI::isInScrollView) realY += FlarialGUI::scrollpos;

    if(FlarialGUI::CursorInRect(x, realY, BottomRoundedWidth, BottomRoundedHeight)) {
        FlarialGUI::lerp(sizes[index].x, Constraints::RelativeConstraint(0.198f, "height", true),
                         0.15f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(sizes[index].y, Constraints::RelativeConstraint(0.149f, "height", true), 0.15f * FlarialGUI::frameFactor);

         diffX = (sizes[index].x - Constraints::RelativeConstraint(0.19f, "height", true)) / 2.0f;
         diffY = (sizes[index].y - Constraints::RelativeConstraint(0.141f, "height", true)) / 2.0f;

         FlarialGUI::ShadowRect(D2D1::RoundedRect(D2D1::RectF(x, y, x + BottomRoundedWidth, y + BottomRoundedHeight), round.x, round.x));
    } else {
        FlarialGUI::lerp(sizes[index].x, Constraints::RelativeConstraint(0.19f, "height", true),
                         0.15f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(sizes[index].y, Constraints::RelativeConstraint(0.141f, "height", true), 0.15f * FlarialGUI::frameFactor);

        diffX = (sizes[index].x - Constraints::RelativeConstraint(0.19f, "height", true)) / 2.0f;
        diffY = (sizes[index].y - Constraints::RelativeConstraint(0.141f, "height", true)) / 2.0f;
    }



    x -= diffX;
    y -= diffY;

    FlarialGUI::RoundedRect(x, y, D2D1::ColorF(47.0f/255.0f, 32.0f/255.0f, 34.0f/255.0f), BottomRoundedWidth, BottomRoundedHeight, round.x, round.x);


    // Top rounded rect

    float TopRoundedHeight = Constraints::SpacingConstraint(sizes[index].y, 0.635f);
    FlarialGUI::RoundedRectOnlyTopCorner(x, y, D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), BottomRoundedWidth, TopRoundedHeight, round.x, round.x);

    FlarialGUI::PushSize(x, y, BottomRoundedWidth, BottomRoundedHeight);

    // Mod Name
    float textx = Constraints::PercentageConstraint(0.213, "left");
    float texty = Constraints::PercentageConstraint(0.415, "top");
    float textWidth = Constraints::RelativeConstraint(0.8);
    float textHeight = Constraints::RelativeConstraint(0.2);

    FlarialGUI::FlarialText(textx, texty, FlarialGUI::to_wide(mod->name).c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight);

    // Mod icon

    float modiconx = Constraints::PercentageConstraint(0.40, "left");
    float modicony = Constraints::PercentageConstraint(0.11, "top");

    float paddingSize = Constraints::RelativeConstraint(0.28);
    FlarialGUI::RoundedRect(modiconx, modicony, D2D1::ColorF(63.0f/255.0f, 42.0f/255.0f, 45.0f/255.0f), paddingSize, paddingSize, 7.5, 7.5);

    paddingSize = Constraints::RelativeConstraint(0.20);
    modiconx = Constraints::PercentageConstraint(0.43, "left");
    modicony = Constraints::PercentageConstraint(0.15, "top");

    // enabled / disabled button

    std::string text;
    text = mod->settings.getSettingByName<bool>("enabled")->value ? "Enabled" : "Disabled";
    D2D1_COLOR_F enabledColor = D2D1::ColorF(24.0f / 255.0f, 136.0f / 255.0f, 48.0f / 255.0f);
    D2D1_COLOR_F disabledColor = D2D1::ColorF(139.0f / 255.0f, 27.0f / 255.0f, 37.0f / 255.0f);

    if(mod->settings.getSettingByName<bool>("enabled")->value) {

        FlarialGUI::buttonColors[index] = FlarialGUI::LerpColor(FlarialGUI::buttonColors[index], enabledColor, 0.15f * FlarialGUI::frameFactor);

    } else {

        FlarialGUI::buttonColors[index] = FlarialGUI::LerpColor(FlarialGUI::buttonColors[index],  disabledColor, 0.15f * FlarialGUI::frameFactor);

    }

    float buttonWidth = Constraints::RelativeConstraint(0.71, "width");
    float buttonHeight = Constraints::RelativeConstraint(0.27);

    float buttonx = Constraints::PercentageConstraint(0.04, "right");
    float buttony = Constraints::PercentageConstraint(0.05, "bottom");

    round = Constraints::RoundingConstraint(26, 26);

    if(FlarialGUI::RoundedButton(buttonx - buttonWidth, buttony - buttonHeight, FlarialGUI::buttonColors[index], D2D1::ColorF(D2D1::ColorF::White), FlarialGUI::to_wide(text).c_str(), buttonWidth, buttonHeight, round.x, round.x)) mod->settings.getSettingByName<bool>("enabled")->value = !mod->settings.getSettingByName<bool>("enabled")->value;

    // Settings Button
    float settingswidth = Constraints::RelativeConstraint(0.17);
    float iconwidth = Constraints::RelativeConstraint(0.10);
    float settingsheightspac = Constraints::SpacingConstraint(0.31, settingswidth);

    float paddingwidth = Constraints::RelativeConstraint(0.26);
    float paddingspacing = Constraints::SpacingConstraint(4.84, paddingwidth);
    float paddingheightspac = Constraints::SpacingConstraint(0.05, paddingwidth);

    round = Constraints::RoundingConstraint(20, 20);

    float settingx = Constraints::PercentageConstraint(0.025, "left");
    float settingx2 = Constraints::PercentageConstraint(0.059, "left");

    FlarialGUI::RoundedRect(settingx, (buttony - paddingwidth) - paddingheightspac, D2D1::ColorF(63.0f / 255.0f, 42.0f / 255.0f, 45.0f / 255.0f), paddingwidth, paddingwidth, round.x, round.x);
    FlarialGUI::RoundedRectWithImageAndText(index, settingx2, (buttony - settingswidth) - settingsheightspac, settingswidth, settingswidth, D2D1::ColorF(112.0f / 255.0f, 93.0f / 255.0f, 96.0f / 255.0f), "\\Flarial\\assets\\gear.png", iconwidth, iconwidth, L"");


    if (!iconpath.empty() && images[mod->name] == nullptr) {

        std::string among = Utils::getRoamingPath() + "\\" + iconpath;
        FlarialGUI::LoadImageFromFile(FlarialGUI::to_wide(among).c_str(), &images[mod->name]);

    } else if (images[mod->name] != nullptr) {

        if (FlarialGUI::isInScrollView) {
            modicony += FlarialGUI::scrollpos;
        }

        D2D::context->DrawBitmap(images[mod->name], D2D1::RectF(modiconx, modicony, modiconx + paddingSize, modicony + paddingSize));
    }

    if(FlarialGUI::isInScrollView)
        buttony += FlarialGUI::scrollpos;

    if (FlarialGUI::CursorInRect(buttonx - paddingspacing, (buttony - paddingwidth) - paddingheightspac,  paddingwidth, paddingwidth) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        ClickGUIRenderer::page.type = "settings";
        ClickGUIRenderer::page.module = mod->name;
        FlarialGUI::scrollpos = 0;
        FlarialGUI::barscrollpos = 0;
        GUIMouseListener::accumilatedPos = 0;
        GUIMouseListener::accumilatedBarPos = 0;
    }

    FlarialGUI::PopSize();

}

void ClickGUIElements::convertImageToBitmap(ID2D1Image* pImg, D2D1_SIZE_U size)
{
    ID2D1Image* oldTarget = NULL;
    ID2D1Bitmap1* targetBitmap = NULL;

    //Create a Bitmap with "D2D1_BITMAP_OPTIONS_TARGET"
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
                    D2D1_BITMAP_OPTIONS_TARGET,
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
    D2D::context->CreateBitmap(size, 0, 0, bitmapProperties, &targetBitmap);



    //Save current Target, replace by ID2D1Bitmap
    D2D::context->GetTarget(&oldTarget);
    D2D::context->SetTarget(targetBitmap);
    //Draw Image on Target (if currently not drawing also call Begin/EndDraw)
    D2D::context->DrawImage(pImg);

    //Set previous Target
    D2D::context->SetTarget(oldTarget);
}

