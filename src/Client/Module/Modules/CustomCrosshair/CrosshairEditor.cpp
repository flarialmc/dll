#include "CustomCrosshair.hpp"
#include "Manager.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "Scripting/ScriptLibs/AudioLib.hpp"

#include <ImGui/stb.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

CrosshairImage::CrosshairImage(std::vector<bool> Data, int Size)
{
    this->PixelData = Data;
    this->Size = Size;
    valid = true;
};

CrosshairImage::CrosshairImage(std::string Path)
{
    int w;
    int h;
    int comp;
    unsigned char* image = stbi_load(Path.c_str(), &w, &h, &comp, STBI_rgb_alpha);

    if (w != 16 or h != 16) return;

    Size = w;

    PixelData.clear();

    for (int i = 0; i < Size*Size*4; i+=4)
    {
        if (image[i] == 255)
        {
            PixelData.push_back(true);
        }
        else
        {
            PixelData.push_back(false);
        }
    }

    if (PixelData.size() == Size*Size)
        valid = true;
}

const unsigned char* CrosshairImage::getImageData()
{
    // Allocate a buffer for RGBA data (4 bytes per pixel)
    unsigned char* data = new unsigned char[Size * Size * 4];

    for (int i = 0; i < Size * Size; i++) {
        if (PixelData[i]) {
            // White pixel (RGBA: 255, 255, 255, 255)
            data[i * 4 + 0] = 255; // R
            data[i * 4 + 1] = 255; // G
            data[i * 4 + 2] = 255; // B
            data[i * 4 + 3] = 255; // A
        } else {
            // Transparent pixel (RGBA: 0, 0, 0, 0)
            data[i * 4 + 0] = 0; // R
            data[i * 4 + 1] = 0; // G
            data[i * 4 + 2] = 0; // B
            data[i * 4 + 3] = 0; // A
        }
    }

    return data;
}

void CrosshairImage::SaveImage(std::string name)
{
    // Get the image data
    const unsigned char* data = getImageData();

    // Use stb_image_write to save the PNG
    stbi_write_png((Utils::getClientPath() + "//Crosshairs//" + name + ".png").c_str(), Size, Size, 4, data, Size * 4);

    // Clean up the allocated memory
    delete[] data;
}

bool CreateCrosshairWindow = false;

void CustomCrosshair::CrosshairEditorWindow()
{
    if (blankWindow) {
        FlarialGUI::PushImClipRect(ImVec2(0, 0), ImVec2(Constraints::RelativeConstraint(1, "width", true), Constraints::RelativeConstraint(1, "height", true)), true);

        // Semi-transparent black background
        FlarialGUI::RoundedRect(
            0,
            0,
            D2D1::ColorF(D2D1::ColorF::Black, 0.75),
            Constraints::RelativeConstraint(1, "width", true),
            Constraints::RelativeConstraint(1, "height", true), 0, 0);

        // Centered window
        float rectwidth = Constraints::RelativeConstraint(0.75, "height", true);
        float rectheight = Constraints::RelativeConstraint(0.57, "height", true);
        Vec2<float> center = Constraints::CenterConstraint(rectwidth, rectheight);
        Vec2<float> round = Constraints::RoundingConstraint(45, 45);
        float y = Constraints::PercentageConstraint(0.10, "top");

        D2D1_COLOR_F colorThing = clickgui->getColor("secondary8", "ClickGUI");
        D2D1_COLOR_F anotherColor = clickgui->getColor("secondary2", "ClickGUI");
        D2D1_COLOR_F anotheranotherColorthing = clickgui->getColor("primary1", "ClickGUI");
        D2D1_COLOR_F anotheranotheranotherColorthing = clickgui->getColor("primary3", "ClickGUI");
        D2D1_COLOR_F textCol = clickgui->getColor("globalText", "ClickGUI");

        FlarialGUI::RoundedHollowRect(center.x, center.y, Constraints::RelativeConstraint(0.01, "height", true),
            colorThing, rectwidth, rectheight, round.x, round.x);
        FlarialGUI::RoundedRect(center.x, center.y, anotherColor, rectwidth, rectheight, round.x, round.x);

        // Initialize pixel data if empty

        bool locked = false;

        CrosshairImage* crosshair;
        if (CurrentSelectedCrosshair == "" or crosshairs[CurrentSelectedCrosshair] == nullptr)
        {
            crosshair = new CrosshairImage();
            locked = true;
            CurrentSelectedCrosshair = "";
        }
        else
            crosshair = crosshairs[CurrentSelectedCrosshair];

        // 16x16 Grid for editor
        float gridSize = Constraints::RelativeConstraint(0.38, "height", true);
        float cellSize = gridSize / 16.0f;
        float gridX = center.x + Constraints::RelativeConstraint(0.02, "height", true);
        float gridY = center.y + Constraints::RelativeConstraint(0.02, "height", true);

        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                float cellX = gridX + j * cellSize;
                float cellY = gridY + i * cellSize;

                // Draw cell (white if true, gray if false)
                D2D1_COLOR_F cellColor = crosshair->PixelData[i * 16 + j] ?
                    D2D1::ColorF(D2D1::ColorF::White, 1.0f) :
                    D2D1::ColorF(D2D1::ColorF::Gray, 1.0f);
                if (locked)
                {
                    cellColor = D2D1_COLOR_F(0.3, 0.3, 0.3, 1.0);
                }
                FlarialGUI::RoundedRect(cellX, cellY, cellColor, cellSize, cellSize, 0, 0);
                FlarialGUI::RoundedHollowRect(cellX, cellY, 2, D2D1::ColorF(D2D1::ColorF::Black, 1), cellSize, cellSize, 0, 0);

                // Handle left click/drag to set pixel
                if (FlarialGUI::CursorInRect(cellX, cellY, cellSize, cellSize) &&
                    MC::lastMouseButton == MouseButton::Left && MC::held and !locked and !CreateCrosshairWindow) {
                    crosshair->PixelData[i * 16 + j] = true;
                }

                // Handle right click/drag to clear pixel
                if (FlarialGUI::CursorInRect(cellX, cellY, cellSize, cellSize) &&
                    MC::lastMouseButton == MouseButton::Right && MC::held and !locked and !CreateCrosshairWindow) {
                    crosshair->PixelData[i * 16 + j] = false;
                }
            }
        }

        y += Constraints::RelativeConstraint(0.55, "height");

        // Close button
        float buttonWidth = (gridSize/2) * 0.975;
        float buttonHeight = Constraints::RelativeConstraint(0.05f, "height", true);

        if (FlarialGUI::RoundedButton(3, gridX, y,
            colorThing, textCol, L"Close", buttonWidth, buttonHeight, round.x/2, round.x/2)) {

            for (const auto& entry : std::filesystem::directory_iterator(Utils::getClientPath() + "//Crosshairs"))
                std::filesystem::remove_all(entry.path());

            for (auto ch : crosshairs)
            {
                if (ch.second != nullptr and ch.first != "")
                    ch.second->SaveImage(ch.first);
            }

            blankWindow = false;
        }

        // Save button
        if (FlarialGUI::RoundedButton(4, gridX,
            y + buttonHeight + Constraints::RelativeConstraint(0.01, "height"),
            colorThing, textCol, L"Create", buttonWidth, buttonHeight, round.x/2, round.x/2)) {

            CreateCrosshairWindow = true;
        }

        if (FlarialGUI::RoundedButton(5, gridX + buttonWidth + ((gridSize/2) * 0.05), y,
            colorThing, textCol, L"Delete", buttonWidth, buttonHeight, round.x/2, round.x/2))
        {
            auto it = crosshairs.find(CurrentSelectedCrosshair);

            if (it != crosshairs.end()) {
                delete it->second;
                crosshairs.erase(it);
            }

            if (settings.getSettingByName<std::string>("CurrentCrosshair")->value == CurrentSelectedCrosshair)
            {
                settings.getSettingByName<std::string>("CurrentCrosshair")->value = "";
                CrosshairReloaded = true;
            }
        }

        if (FlarialGUI::RoundedButton(6, gridX + buttonWidth + ((gridSize/2) * 0.05),
            y + buttonHeight + Constraints::RelativeConstraint(0.01, "height"),
            colorThing, textCol, L"Load", buttonWidth, buttonHeight, round.x/2, round.x/2)) {

            settings.getSettingByName<std::string>("CurrentCrosshair")->value = CurrentSelectedCrosshair;
            crosshair->SaveImage(settings.getSettingByName<std::string>("CurrentCrosshair")->value);
            CrosshairReloaded = true;
            }

        float CrosshairSelectorX = gridX + Constraints::RelativeConstraint(0.425, "height", true) - Constraints::RelativeConstraint(0.015, "height", true);
        float CrosshairSelectorY = gridY;
        ImVec2 CrosshairSelectorSize = ImVec2(gridSize/1.3 + Constraints::RelativeConstraint(0.015, "height", true), gridSize * 1.5);

        float CrosshairRectWidth = CrosshairSelectorSize.x * 0.9;
        float CrosshairRectHeight = CrosshairSelectorSize.y * 0.1;

        float Spacing = CrosshairSelectorSize.x * 0.05;

        FlarialGUI::RoundedRect(CrosshairSelectorX, CrosshairSelectorY, colorThing, CrosshairSelectorSize.x, rectheight - Constraints::RelativeConstraint(0.04, "height", true), round.x/1.5, round.x/1.5);

        FlarialGUI::SetScrollView(CrosshairSelectorX, CrosshairSelectorY, CrosshairSelectorSize.x, rectheight - Constraints::RelativeConstraint(0.04, "height", true));

        float MelivnIsGayAsFuckLikeSuckingCocksWhileRidingBigBlackMenDuringThePrideMonthWithAGayFlagUpHisAss = CrosshairSelectorY + Spacing;

        for (auto ch : crosshairs)
        {
            if (ch.second == nullptr or ch.first == "") continue;
            if (FlarialGUI::CursorInRect(CrosshairSelectorX + Spacing, MelivnIsGayAsFuckLikeSuckingCocksWhileRidingBigBlackMenDuringThePrideMonthWithAGayFlagUpHisAss, CrosshairRectWidth, CrosshairRectHeight)  && MC::mouseButton == MouseButton::Left && !MC::held and !CreateCrosshairWindow)
            {
                CurrentSelectedCrosshair = ch.first;
            }

            auto color = anotheranotheranotherColorthing;
            if (ch.first == CurrentSelectedCrosshair) color = anotheranotherColorthing;
            FlarialGUI::RoundedRect(CrosshairSelectorX + Spacing, MelivnIsGayAsFuckLikeSuckingCocksWhileRidingBigBlackMenDuringThePrideMonthWithAGayFlagUpHisAss, color, CrosshairRectWidth, CrosshairRectHeight, round.x/3, round.x/3);
            FlarialGUI::FlarialTextWithFont(CrosshairSelectorX + Spacing, MelivnIsGayAsFuckLikeSuckingCocksWhileRidingBigBlackMenDuringThePrideMonthWithAGayFlagUpHisAss, FlarialGUI::to_wide(ch.first).c_str(), CrosshairRectWidth, CrosshairRectHeight, DWRITE_TEXT_ALIGNMENT_CENTER, CrosshairRectWidth/1.5 , DWRITE_FONT_WEIGHT_NORMAL);

            MelivnIsGayAsFuckLikeSuckingCocksWhileRidingBigBlackMenDuringThePrideMonthWithAGayFlagUpHisAss += Spacing + CrosshairRectHeight;
        }

        FlarialGUI::UnsetScrollView();

        if (CreateCrosshairWindow)
        {
            FlarialGUI::RoundedRect(
            0,
            0,
            D2D1::ColorF(D2D1::ColorF::Black, 0.75),
            Constraints::RelativeConstraint(1, "width", true),
            Constraints::RelativeConstraint(1, "height", true), 0, 0);

            rectwidth = Constraints::RelativeConstraint(0.3, "height", true);
            rectheight = Constraints::RelativeConstraint(0.168, "height", true);
            center = Constraints::CenterConstraint(rectwidth, rectheight);

            FlarialGUI::RoundedHollowRect(center.x, center.y, Constraints::RelativeConstraint(0.01, "height", true),
            colorThing, rectwidth, rectheight, round.x, round.x);
            FlarialGUI::RoundedRect(center.x, center.y, anotherColor, rectwidth, rectheight, round.x, round.x);

            float TextBoxX = center.x + Constraints::RelativeConstraint(0.03, "height", true);
            float TextBoxY = center.y + Constraints::RelativeConstraint(0.038, "height", true);

            float TextBoxHeight = Constraints::RelativeConstraint(0.035, "height", true);

            FlarialGUI::TextBoxVisual(6996, CrosshairText, 0, TextBoxX, TextBoxY, "");

            if (FlarialGUI::RoundedButton(7, TextBoxX, TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), colorThing, textCol, L"Create", Constraints::RelativeConstraint(0.15, "height", true), TextBoxHeight*1.8, round.x/2, round.x/2))
            {
                crosshairs[CrosshairText] = new CrosshairImage();
                CurrentSelectedCrosshair = CrosshairText;
                CrosshairText = "";
                CreateCrosshairWindow = false;
            }
        }

        FlarialGUI::PopImClipRect();
        FlarialGUI::PushSize(center.x, center.y, rectwidth, rectheight);
        FlarialGUI::PopSize();
        FlarialGUI::UnsetWindowRect();
    }
}