#include "CustomCrosshair.hpp"
#include "Manager.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "Scripting/ScriptLibs/AudioLib.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

CrosshairImage::CrosshairImage(std::vector<bool> Data, int Size)
{
    this->PixelData = Data;
    this->Size = Size;
};

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
    stbi_write_png(name.c_str(), Size, Size, 4, data, Size * 4);

    // Clean up the allocated memory
    delete[] data;
}

void CustomCrosshair::CrosshairEditorWindow()
{
    if (blankWindow) {
        // Semi-transparent black background
        FlarialGUI::RoundedRect(0, 0, D2D1::ColorF(D2D1::ColorF::Black, 0.75),
            Constraints::RelativeConstraint(1.5, "width", true),
            Constraints::RelativeConstraint(1.5, "height", true), 0, 0);

        // Centered window
        float rectwidth = Constraints::RelativeConstraint(0.55, "height", true);
        float rectheight = Constraints::RelativeConstraint(0.45, "height", true);
        Vec2<float> center = Constraints::CenterConstraint(rectwidth, rectheight);
        Vec2<float> round = Constraints::RoundingConstraint(45, 45);
        float y = Constraints::PercentageConstraint(0.10, "top");

        D2D1_COLOR_F colorThing = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
        colorThing.a = o_colors_secondary2;

        D2D1_COLOR_F anotherColor = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
        anotherColor.a = o_colors_secondary1;

        D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
        textCol.a = o_colors_text;

        FlarialGUI::RoundedHollowRect(center.x, center.y, Constraints::RelativeConstraint(0.01, "height", true),
            colorThing, rectwidth, rectheight, round.x, round.x);
        FlarialGUI::RoundedRect(center.x, center.y, anotherColor, rectwidth, rectheight, round.x, round.x);

        // Initialize pixel data if empty
        static CrosshairImage crosshair(std::vector<bool>(256, false), 16);

        // 16x16 Grid for editor
        float gridSize = Constraints::RelativeConstraint(0.35, "height", true);
        float cellSize = gridSize / 16.0f;
        float gridX = center.x + Constraints::RelativeConstraint(0.05, "height", true);
        float gridY = center.y + Constraints::RelativeConstraint(0.05, "height", true);

        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                float cellX = gridX + j * cellSize;
                float cellY = gridY + i * cellSize;

                // Draw cell (white if true, gray if false)
                D2D1_COLOR_F cellColor = crosshair.PixelData[i * 16 + j] ?
                    D2D1::ColorF(D2D1::ColorF::White, 1.0f) :
                    D2D1::ColorF(D2D1::ColorF::Gray, 0.5f);
                FlarialGUI::RoundedRect(cellX, cellY, cellColor, cellSize, cellSize, 2, 2);

                // Handle left click/drag to set pixel
                if (FlarialGUI::CursorInRect(cellX, cellY, cellSize, cellSize) &&
                    MC::lastMouseButton == MouseButton::Left && MC::held) {
                    crosshair.PixelData[i * 16 + j] = true;
                }

                // Handle right click/drag to clear pixel
                if (FlarialGUI::CursorInRect(cellX, cellY, cellSize, cellSize) &&
                    MC::lastMouseButton == MouseButton::Right && MC::held) {
                    crosshair.PixelData[i * 16 + j] = false;
                }
            }
        }

        // Close button
        float buttonWidth = Constraints::RelativeConstraint(0.25f, "width");
        float buttonHeight = Constraints::RelativeConstraint(0.13f, "height");
        if (FlarialGUI::RoundedButton(0, Constraints::PercentageConstraint(0.07, "right") - buttonWidth, y,
            colorThing, textCol, L"Close", buttonWidth, buttonHeight, round.x, round.x)) {
            blankWindow = false;
        }

        // Save button
        if (FlarialGUI::RoundedButton(1, Constraints::PercentageConstraint(0.07, "right") - buttonWidth,
            y + buttonHeight + Constraints::RelativeConstraint(0.02, "height"),
            colorThing, textCol, L"Save", buttonWidth, buttonHeight, round.x, round.x)) {
            crosshair.SaveImage(Utils::getAssetsPath() + "\\ch1.png");
        }

        FlarialGUI::PushSize(center.x, center.y, rectwidth, rectheight);
        FlarialGUI::PopSize();
        FlarialGUI::UnsetWindowRect();
    }
}