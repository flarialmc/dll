#include "CustomCrosshair.hpp"
#include "Manager.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "Scripting/ScriptLibs/AudioLib.hpp"

#include <ImGui/stb.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

float BrushSize = 1;
int CurrentSelectedCreateCrosshairSize = 16;

CrosshairImage::CrosshairImage(int Size)
{
    this->Size = Size;
    valid = true;
    // Initialize PixelData with transparent pixels
    PixelData.resize(Size * Size, Vec4<int>(0, 0, 0, 0));
}

CrosshairImage::CrosshairImage(std::string Path)
{
    int w;
    int h;
    int comp;
    unsigned char* image = stbi_load(Path.c_str(), &w, &h, &comp, STBI_rgb_alpha);

    if (w != h) {
        valid = false;
        if (image) stbi_image_free(image);
        return;
    }

    Size = w;
    PixelData.clear();

    for (int i = 0; i < Size * Size * 4; i += 4)
    {
        PixelData.push_back(Vec4<int>(image[i], image[i + 1], image[i + 2], image[i + 3]));
    }

    if (image) stbi_image_free(image);

    if (PixelData.size() == Size * Size)
        valid = true;
    else
        valid = false;
}

const unsigned char* CrosshairImage::getImageData()
{
    // Ensure PixelData is properly sized
    if (PixelData.size() != Size * Size) {
        PixelData.resize(Size * Size, Vec4<int>(0, 0, 0, 0));
    }

    unsigned char* data = new unsigned char[Size * Size * 4];

    for (int i = 0; i < Size * Size; i++) {
        data[i * 4 + 0] = PixelData[i].x; // R
        data[i * 4 + 1] = PixelData[i].y; // G
        data[i * 4 + 2] = PixelData[i].z; // B
        data[i * 4 + 3] = PixelData[i].w; // A
    }

    return data;
}

void CrosshairImage::SaveImage(std::string name)
{
    // Get the image data
    const unsigned char* data = getImageData();

    // Use stb_image_write to save the PNG
    int result = stbi_write_png((Utils::getClientPath() + "//Crosshairs//" + name + ".png").c_str(), Size, Size, 4, data, Size * 4);

    // Clean up the allocated memory
    delete[] data;

    // Optional: Log error if save fails
    if (!result) {
        // Add error handling (e.g., log to console or UI)
        // std::cerr << "Failed to save crosshair: " << name << std::endl;
    }
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
        float rectwidth = Constraints::RelativeConstraint(0.9, "height", true);
        float rectheight = Constraints::RelativeConstraint(0.72, "height", true);
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

        // Initialize crosshair
        bool locked = false;
        CrosshairImage* crosshair;
        if (CurrentSelectedCrosshair == "" || crosshairs[CurrentSelectedCrosshair] == nullptr)
        {
            crosshair = new CrosshairImage(16);
            CurrentSelectedCrosshair = "";
        }
        else
        {
            crosshair = crosshairs[CurrentSelectedCrosshair];
        }

        // 16x16 Grid for editor
        float gridSize = Constraints::RelativeConstraint(0.5, "height", true);
        float cellSize = gridSize / crosshair->Size;
        float gridX = center.x + Constraints::RelativeConstraint(0.02, "height", true);
        float gridY = center.y + Constraints::RelativeConstraint(0.02, "height", true);

        FlarialGUI::image(IDR_TRANSPARENT_PNG, D2D1::RectF(
            gridX,
            gridY,
            gridX + gridSize,
            gridY + gridSize
        ),
        "PNG",
        true,
        ImColor(165, 165, 165, 255));

        for (int i = 0; i < crosshair->Size; i++) {
            for (int j = 0; j < crosshair->Size; j++) {
                float cellX = gridX + j * cellSize;
                float cellY = gridY + i * cellSize;

                // Ensure PixelData is valid
                Vec4<int> CellColor(0, 0, 0, 0);
                CellColor = crosshair->PixelData[i * crosshair->Size + j];

                D2D1_COLOR_F cellColor = D2D1::ColorF(CellColor.x / 255.0f, CellColor.y / 255.0f, CellColor.z / 255.0f, CellColor.w / 255.0f);
                if (locked) {
                    cellColor = D2D1_COLOR_F(0.3, 0.3, 0.3, 1.0);
                }
                FlarialGUI::RoundedRect(cellX, cellY, cellColor, cellSize, cellSize, 0, 0);

                // Handle mouse interaction with circular brush
                if (FlarialGUI::CursorInRect(cellX, cellY, cellSize, cellSize) &&
                    MC::held && !locked && !CreateCrosshairWindow && !FlarialGUI::activeColorPickerWindows) {
                    if (crosshair->PixelData.size() == crosshair->Size * crosshair->Size) {
                        // Calculate brush radius (BrushSize is the diameter)
                        float radius = BrushSize / 2.0f;
                        int brushRadius = static_cast<int>(std::ceil(radius));
                        for (int di = -brushRadius; di <= brushRadius; di++) {
                            for (int dj = -brushRadius; dj <= brushRadius; dj++) {
                                // Check if pixel is within circular radius
                                float distance = std::sqrt(static_cast<float>(di * di + dj * dj));
                                if (distance <= radius) {
                                    int ni = i + di;
                                    int nj = j + dj;
                                    // Check boundaries
                                    if (ni >= 0 && ni < crosshair->Size && nj >= 0 && nj < crosshair->Size) {
                                        if (MC::lastMouseButton == MouseButton::Left) {
                                            // Set color for left click/drag
                                            crosshair->PixelData[ni * crosshair->Size + nj] = Vec4<int>(
                                                getColor("CurrentSelectedColor").r * 255,
                                                getColor("CurrentSelectedColor").g * 255,
                                                getColor("CurrentSelectedColor").b * 255,
                                                getColor("CurrentSelectedColor").a * 255
                                            );
                                        } else if (MC::lastMouseButton == MouseButton::Right) {
                                            // Clear pixel for right click/drag
                                            crosshair->PixelData[ni * crosshair->Size + nj] = Vec4<int>(0, 0, 0, 0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (getOps<bool>("ShowGridLines"))
            for (int i = 1; i < crosshair->Size; i++)
            {
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(gridX + i * cellSize, gridY), ImVec2(gridX + i * cellSize, gridY + gridSize), ImColor(0, 0, 0, 180), 2);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(gridX, gridY+ i * cellSize), ImVec2(gridX + gridSize, gridY + i * cellSize), ImColor(0, 0, 0, 180), 2);
            }

        if (getOps<bool>("HighlightMiddleLine"))
        {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(gridX + (gridSize/2), gridY), ImVec2(gridX + (gridSize/2), gridY + gridSize), ImColor(255, 0, 0, 255), 4);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(gridX, gridY + (gridSize/2)), ImVec2(gridX + gridSize, gridY + (gridSize/2)), ImColor(255, 0, 0, 255), 4);
        }

        FlarialGUI::RoundedHollowRect(gridX, gridY, Constraints::RelativeConstraint(0.01, "height", true), anotherColor, gridSize, gridSize, round.x / 3, round.x / 3);



        FlarialGUI::RoundedRect(gridX, gridY + gridSize + Constraints::RelativeConstraint(0.022, "height", true), colorThing, gridSize, rectheight - Constraints::RelativeConstraint(0.04, "height", true) - gridSize - Constraints::RelativeConstraint(0.02, "height", true), round.x/2, round.x/2);

        y = gridY + gridSize + Constraints::RelativeConstraint(0.05, "height", true);

        // Close button
        float buttonWidth = (gridSize / 2) * 0.85;
        float buttonHeight = Constraints::RelativeConstraint(0.05f, "height", true);

        if (FlarialGUI::RoundedButton(5, gridX + ((gridSize / 2) * 0.1), y + buttonHeight + Constraints::RelativeConstraint(0.01, "height"),
            anotherColor, textCol, L"Close", buttonWidth, buttonHeight, round.x / 2, round.x / 2)) {

            // Save all crosshairs
            for (const auto& entry : std::filesystem::directory_iterator(Utils::getClientPath() + "//Crosshairs"))
                std::filesystem::remove_all(entry.path());

            for (auto& ch : crosshairs) {
                if (ch.second != nullptr && !ch.first.empty()) {
                    ch.second->SaveImage(ch.first);
                }
            }

            blankWindow = false;
        }

        if (FlarialGUI::RoundedButton(6, gridX + buttonWidth + ((gridSize / 2) * 0.2),
            y + buttonHeight + Constraints::RelativeConstraint(0.01, "height"),
            anotherColor, textCol, L"Load", buttonWidth, buttonHeight, round.x / 2, round.x / 2)) {

            settings.getSettingByName<std::string>("CurrentCrosshair")->value = CurrentSelectedCrosshair;
            if (crosshair != nullptr && !CurrentSelectedCrosshair.empty()) {
                crosshair->SaveImage(settings.getSettingByName<std::string>("CurrentCrosshair")->value);
            }
            CrosshairReloaded = true;
        }

        float CrosshairSelectorX = gridX + gridSize + Constraints::RelativeConstraint(0.022, "height", true);
        float CrosshairSelectorY = gridY;
        ImVec2 CrosshairSelectorSize = ImVec2(gridSize / 1.6 + Constraints::RelativeConstraint(0.02, "height", true), gridSize * 1.5);

        float CrosshairRectWidth = CrosshairSelectorSize.x * 0.9;
        float CrosshairRectHeight = CrosshairSelectorSize.y * 0.07;

        float Spacing = CrosshairSelectorSize.x * 0.05;

        FlarialGUI::RoundedRect(CrosshairSelectorX, CrosshairSelectorY, colorThing, CrosshairSelectorSize.x, rectheight - Constraints::RelativeConstraint(0.04, "height", true), round.x / 1.5, round.x / 1.5);

        FlarialGUI::SetScrollView(CrosshairSelectorX, CrosshairSelectorY, CrosshairSelectorSize.x, rectheight - Constraints::RelativeConstraint(0.04, "height", true) - (Spacing * 2) - CrosshairRectHeight);

        float CrosshairsYPos = CrosshairSelectorY + Spacing;

        for (auto ch : crosshairs)
        {
            if (ch.second == nullptr || ch.first.empty()) continue;
            if (FlarialGUI::CursorInRect(CrosshairSelectorX + Spacing, CrosshairsYPos, CrosshairRectWidth, CrosshairRectHeight) && MC::mouseButton == MouseButton::Left && !MC::held && !CreateCrosshairWindow and !FlarialGUI::activeColorPickerWindows)
            {
                CurrentSelectedCrosshair = ch.first;
            }

            auto color = anotheranotheranotherColorthing;
            if (ch.first == CurrentSelectedCrosshair) color = anotheranotherColorthing;
            ImVec2 CrosshairRectClippingPos = ImVec2(CrosshairSelectorX + Spacing, CrosshairsYPos);
            if (FlarialGUI::isInScrollView)
            {
                CrosshairRectClippingPos.y += FlarialGUI::scrollpos;
            }
            FlarialGUI::PushImClipRect(CrosshairRectClippingPos, ImVec2(CrosshairRectWidth, CrosshairRectHeight));
            FlarialGUI::RoundedRect(CrosshairSelectorX + Spacing, CrosshairsYPos, color, CrosshairRectWidth, CrosshairRectHeight, round.x / 3, round.x / 3);
            FlarialGUI::FlarialTextWithFont(CrosshairSelectorX + Spacing, CrosshairsYPos, FlarialGUI::to_wide(ch.first).c_str(), CrosshairRectWidth, CrosshairRectHeight, DWRITE_TEXT_ALIGNMENT_CENTER, CrosshairRectWidth / 1.5, DWRITE_FONT_WEIGHT_NORMAL);
            FlarialGUI::PopImClipRect();

            CrosshairsYPos += Spacing + CrosshairRectHeight;
        }

        FlarialGUI::UnsetScrollView();

        float ButtonPosX = CrosshairSelectorX + Spacing;
        float ButtonPosY = rectheight - Constraints::RelativeConstraint(0.04, "height", true) - Spacing - CrosshairRectHeight + CrosshairSelectorY;

        if (FlarialGUI::RoundedButton(7, ButtonPosX, ButtonPosY,
            anotheranotherColorthing, textCol, L"Create", CrosshairRectWidth - CrosshairRectHeight - Spacing, CrosshairRectHeight, round.x / 2, round.x / 2)) {

            CreateCrosshairWindow = true;
        }

        ButtonPosX += CrosshairRectWidth - CrosshairRectHeight;

        if (FlarialGUI::RoundedButton(8, ButtonPosX, ButtonPosY,
            anotherColor, textCol, L"", CrosshairRectHeight, CrosshairRectHeight, round.x / 2, round.x / 2))
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

        y += Constraints::RelativeConstraint(0.02, "height", true);

        FlarialGUI::image(IDR_DELETE_PNG, D2D1_RECT_F(ButtonPosX + (CrosshairRectHeight * 0.2), ButtonPosY + (CrosshairRectHeight * 0.2), ButtonPosX + (CrosshairRectHeight * 0.8), ButtonPosY + (CrosshairRectHeight * 0.8)));

        float ColorPickerX = gridX + (Spacing*4);

        FlarialGUI::FlarialTextWithFont(gridX + ((gridSize / 2) * 0.2), y - Constraints::RelativeConstraint(0.033, "height", true), L"Brush Color", 3, Constraints::RelativeConstraint(0.02, "height", true), DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.135, "height", true), DWRITE_FONT_WEIGHT_NORMAL);

        FlarialGUI::ColorPicker(876, gridX + ((gridSize / 2) * 0.2), y, "Custom Crosshair", "CurrentSelectedColor");

        if (FlarialGUI::activeColorPickerWindows)
        {
            FlarialGUI::ColorPickers[876].isActive = true;
            FlarialGUI::ColorPickerWindow(876, "Custom Crosshair", "CurrentSelectedColor");
        }

        FlarialGUI::FlarialTextWithFont(ColorPickerX + Constraints::RelativeConstraint(0.0285, "height", true) * 4.125 + (Spacing * 3), y - Constraints::RelativeConstraint(0.033, "height", true), L"Brush Size", 3, Constraints::RelativeConstraint(0.02, "height", true), DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.135, "height", true), DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Slider(34522, ColorPickerX + Constraints::RelativeConstraint(0.0285, "height", true) * 4.125 + (Spacing * 3), y + Constraints::RelativeConstraint(0.004, "height", true), BrushSize, 8, 1);

        if (CreateCrosshairWindow)
        {
            FlarialGUI::RoundedRect(
                0,
                0,
                D2D1::ColorF(D2D1::ColorF::Black, 0.75),
                Constraints::RelativeConstraint(1, "width", true),
                Constraints::RelativeConstraint(1, "height", true), 0, 0);

            rectwidth = Constraints::RelativeConstraint(0.4, "height", true);
            rectheight = Constraints::RelativeConstraint(0.3, "height", true);
            center = Constraints::CenterConstraint(rectwidth, rectheight);

            FlarialGUI::RoundedHollowRect(center.x, center.y, Constraints::RelativeConstraint(0.01, "height", true),
                colorThing, rectwidth, rectheight, round.x, round.x);
            FlarialGUI::RoundedRect(center.x, center.y, anotherColor, rectwidth, rectheight, round.x, round.x);

            float TextBoxX = center.x + Constraints::RelativeConstraint(0.03, "height", true);
            float TextBoxY = center.y + Constraints::RelativeConstraint(0.05, "height", true);

            float TextBoxHeight = Constraints::RelativeConstraint(0.035, "height", true);

            FlarialGUI::FlarialTextWithFont(TextBoxX, TextBoxY - Constraints::RelativeConstraint(0.035, "height", true), L"Crosshair Name", 3, Constraints::RelativeConstraint(0.02, "height", true), DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.135, "height", true), DWRITE_FONT_WEIGHT_NORMAL);
            FlarialGUI::TextBoxVisual(6996, CrosshairText, 0, TextBoxX, TextBoxY + Constraints::RelativeConstraint(0.006, "height", true), "");

            TextBoxY += Constraints::RelativeConstraint(0.037, "height", true);
            float OptionsButtonSize = Constraints::RelativeConstraint(0.06, "height", true);
            float ButtonSpacing = OptionsButtonSize/4;

            FlarialGUI::FlarialTextWithFont(TextBoxX, TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true) - Constraints::RelativeConstraint(0.033, "height", true), L"Crosshair Size", 3, Constraints::RelativeConstraint(0.02, "height", true), DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.135, "height", true), DWRITE_FONT_WEIGHT_NORMAL);

            if (FlarialGUI::RoundedButton(9, TextBoxX, TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), CurrentSelectedCreateCrosshairSize == 16 ? anotheranotherColorthing : colorThing, textCol, L"", OptionsButtonSize, OptionsButtonSize, round.x / 2, round.x / 2))
            {
                CurrentSelectedCreateCrosshairSize = 16;
            }

            FlarialGUI::FlarialTextWithFont(TextBoxX, TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), L"16x", OptionsButtonSize, OptionsButtonSize, DWRITE_TEXT_ALIGNMENT_CENTER, Constraints::RelativeConstraint(0.14, "height", true), DWRITE_FONT_WEIGHT_NORMAL);

            if (FlarialGUI::RoundedButton(10, TextBoxX + (ButtonSpacing + OptionsButtonSize), TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), CurrentSelectedCreateCrosshairSize == 32 ? anotheranotherColorthing : colorThing, textCol, L"", OptionsButtonSize, OptionsButtonSize, round.x / 2, round.x / 2))
            {
                CurrentSelectedCreateCrosshairSize = 32;
            }

            FlarialGUI::FlarialTextWithFont(TextBoxX + (ButtonSpacing + OptionsButtonSize), TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), L"32x", OptionsButtonSize, OptionsButtonSize, DWRITE_TEXT_ALIGNMENT_CENTER, Constraints::RelativeConstraint(0.14, "height", true), DWRITE_FONT_WEIGHT_NORMAL);

            if (FlarialGUI::RoundedButton(11, TextBoxX + ((ButtonSpacing + OptionsButtonSize)*2), TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), CurrentSelectedCreateCrosshairSize == 64 ? anotheranotherColorthing : colorThing, textCol, L"", OptionsButtonSize, OptionsButtonSize, round.x / 2, round.x / 2))
            {
                CurrentSelectedCreateCrosshairSize = 64;
            }

            FlarialGUI::FlarialTextWithFont(TextBoxX + ((ButtonSpacing + OptionsButtonSize)*2), TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), L"64x", OptionsButtonSize, OptionsButtonSize, DWRITE_TEXT_ALIGNMENT_CENTER, Constraints::RelativeConstraint(0.14, "height", true), DWRITE_FONT_WEIGHT_NORMAL);

            TextBoxY += Constraints::RelativeConstraint(0.083, "height", true);

            if (FlarialGUI::RoundedButton(12, TextBoxX, TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), anotheranotherColorthing, textCol, L"Create", Constraints::RelativeConstraint(0.15, "height", true), TextBoxHeight * 1.6, round.x / 2, round.x / 2))
            {
                if (!CrosshairText.empty()) {
                    crosshairs[CrosshairText] = new CrosshairImage(CurrentSelectedCreateCrosshairSize);
                    CurrentSelectedCrosshair = CrosshairText;
                    CrosshairText = "";
                    CreateCrosshairWindow = false;
                }
            }

            if (FlarialGUI::RoundedButton(13, TextBoxX + Constraints::RelativeConstraint(0.15, "height", true) + ButtonSpacing * 1.8, TextBoxY + TextBoxHeight + Constraints::RelativeConstraint(0.01, "height", true), colorThing, textCol, L"Close", Constraints::RelativeConstraint(0.15, "height", true), TextBoxHeight * 1.6, round.x / 2, round.x / 2))
            {
                CreateCrosshairWindow = false;
            }
        }

        FlarialGUI::PopImClipRect();
        FlarialGUI::PushSize(center.x, center.y, rectwidth, rectheight);
        FlarialGUI::PopSize();
        FlarialGUI::UnsetWindowRect();
    }
}