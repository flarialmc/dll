#pragma once
#include "../ClickGUIRenderer.hpp"
#include "ClickGUIElements.hpp"


std::map<std::string, ID2D1Bitmap*> ClickGUIElements::images;
std::vector<D2D1_MATRIX_3X2_F> ClickGUIElements::matrixes;

void ClickGUIElements::ModCard(float x, float y, Module* mod, const std::string iconpath, const int index)
{

    Vec2<float> vec = FlarialGUI::CalculateMovedXY(x, y, 0);
    x = vec.x;
    y = vec.y;

    Vec2<float> round = Constraints::RoundingConstraint(34, 34);

    // Bottom rounded rect
    float BottomRoundedWidth = Constraints::RelativeConstraint(0.19f, "height", true);
    float BottomRoundedHeight = Constraints::RelativeConstraint(0.141f, "height", true);
    FlarialGUI::RoundedRect(x, y, D2D1::ColorF(47.0f/255.0f, 32.0f/255.0f, 34.0f/255.0f), BottomRoundedWidth, BottomRoundedHeight, round.x, round.x);


    // Top rounded rect

    float TopRoundedHeight = Constraints::RelativeConstraint(0.089f, "height", true);
    FlarialGUI::RoundedRectOnlyTopCorner(x, y, D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), BottomRoundedWidth, TopRoundedHeight, round.x, round.x);

    FlarialGUI::PushSize(x, y, BottomRoundedWidth, BottomRoundedHeight);

    // Mod Name
    float textx = Constraints::PercentageConstraint(0.11, "left");
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
    text = mod->enabled ? "Enabled" : "Disabled";
    D2D1_COLOR_F color;
    color = mod->enabled ? D2D1::ColorF(24.0f / 255.0f, 136.0f / 255.0f, 48.0f / 255.0f) : D2D1::ColorF(139.0f / 255.0f, 27.0f / 255.0f, 37.0f / 255.0f);
    float buttonWidth = Constraints::RelativeConstraint(0.97);
    float buttonHeight = Constraints::RelativeConstraint(0.27);

    float buttonx = Constraints::PercentageConstraint(0.04, "right") ;
    float buttony = Constraints::PercentageConstraint(0.05, "bottom") ;

    round = Constraints::RoundingConstraint(26, 26);

    if(FlarialGUI::RoundedButton(buttonx - buttonWidth, buttony - buttonHeight, color, D2D1::ColorF(D2D1::ColorF::White), FlarialGUI::to_wide(text).c_str(), buttonWidth, buttonHeight, round.x, round.x)) mod->enabled = !mod->enabled;

    // Settings Button
    float settingswidth = Constraints::RelativeConstraint(0.17);
    float iconwidth = Constraints::RelativeConstraint(0.10);
    float settingspacing = Constraints::SpacingConstraint(7.12, settingswidth);
    float settingsheightspac = Constraints::SpacingConstraint(0.31, settingswidth);

    float paddingwidth = Constraints::RelativeConstraint(0.26);
    float paddingspacing = Constraints::SpacingConstraint(4.84, paddingwidth);
    float paddingheightspac = Constraints::SpacingConstraint(0.05, paddingwidth);

    round = Constraints::RoundingConstraint(20, 20);

    FlarialGUI::RoundedRect(buttonx - paddingspacing, (buttony - paddingwidth) - paddingheightspac, D2D1::ColorF(63.0f / 255.0f, 42.0f / 255.0f, 45.0f / 255.0f), paddingwidth, paddingwidth, round.x, round.x);
    FlarialGUI::RoundedRectWithImageAndText(buttonx - settingspacing, (buttony - settingswidth) - settingsheightspac, settingswidth, settingswidth, D2D1::ColorF(112.0f / 255.0f, 93.0f / 255.0f, 96.0f / 255.0f), "\\Flarial\\assets\\gear.png", iconwidth, iconwidth, L"");


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

