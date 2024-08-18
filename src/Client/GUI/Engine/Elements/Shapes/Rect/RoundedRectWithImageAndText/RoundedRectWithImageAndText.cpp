#include "../../../../Engine.hpp"
#include "../../../Structs/ImagesClass.hpp"

void FlarialGUI::RoundedRectWithImageAndText(int index, float x, float y, const float width, const float height,
                                             const D2D1_COLOR_F color, const std::string &imagePath,
                                             const float imageWidth, const float imageHeight) {

    float imageY = y;

    if (isInScrollView) {
        y += scrollpos;
        imageY += scrollpos;
    }

    ID2D1SolidColorBrush *brush;

    brush = FlarialGUI::getBrush(color).get();

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), 5, 5);

    D2D::context->FillRoundedRectangle(roundedRect, brush);

    x = x + (width - imageWidth) / 2.0f;
    imageY = imageY + (height - imageHeight) / 2.0f;

    D2D1_RECT_F imagerectf = D2D1::RectF(x, imageY, x + imageWidth, imageY + imageHeight);

    if (ImagesClass::eimages[imagePath] == nullptr) {

        std::string among = Utils::getRoamingPath() + "\\" + imagePath;
        FlarialGUI::LoadImageFromFile(to_wide(among).c_str(), ImagesClass::eimages[imagePath].put());

    } else if (ImagesClass::eimages[imagePath] != nullptr) {

        D2D1_MATRIX_3X2_F oldTransform;
        D2D::context->GetTransform(&oldTransform);

        if (CursorInRect(x, y, width, height)) {

            FlarialGUI::lerp(rotationAngles[index], rotationAngles[index] + 15, 0.5f * FlarialGUI::frameFactor);
        }


        float rotationAngle = rotationAngles[index];// Specify the rotation angle in degrees
        D2D1_POINT_2F rotationCenter = D2D1::Point2F(imagerectf.left + imageWidth / 2.0f, imagerectf.top +
                                                                                          imageHeight /
                                                                                          2.0f);  // Specify the rotation center
        D2D1_MATRIX_3X2_F rotationMatrix = D2D1::Matrix3x2F::Rotation(rotationAngle, rotationCenter);

        D2D1_MATRIX_3X2_F translationMatrix = D2D1::Matrix3x2F::Translation(x, imageY);
        D2D1_MATRIX_3X2_F combinedMatrix = translationMatrix * rotationMatrix;

        D2D::context->SetTransform(combinedMatrix);

        imagerectf = D2D1::RectF(0, 0, imageWidth, imageHeight);

        D2D::context->DrawBitmap(ImagesClass::eimages[imagePath].get(), imagerectf, 1.0, D2D1_INTERPOLATION_MODE_ANISOTROPIC);

        D2D::context->SetTransform(oldTransform);
    }

    /*
    // Draw text
    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    IDWriteTextFormat *textFormat;
    FlarialGUI::writeFactory->CreateTextFormat(LClient::settings.getSettingByName<std::string>("fontname")->value, NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);*/

    /*
    textFormat->Release();
    FlarialGUI::writeFactory->Release();*/
}

void FlarialGUI::RoundedRectWithImageAndText(int index, float x, float y, const float width, const float height,
                                             const D2D1_COLOR_F color, int iconId,
                                             const float imageWidth, const float imageHeight) {

    float imageY = y;

    if (isInScrollView) {
        y += scrollpos;
        imageY += scrollpos;
    }

    ID2D1SolidColorBrush *brush;

    brush = FlarialGUI::getBrush(color).get();

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), 5, 5);

    D2D::context->FillRoundedRectangle(roundedRect, brush);

    x = x + (width - imageWidth) / 2.0f;
    imageY = imageY + (height - imageHeight) / 2.0f;

    D2D1_RECT_F imagerectf = D2D1::RectF(x, imageY, x + imageWidth, imageY + imageHeight);

    if (ImagesClass::images[iconId] == nullptr) {
        FlarialGUI::LoadImageFromResource(iconId, ImagesClass::images[iconId].put());

    } else if (ImagesClass::images[iconId] != nullptr) {

        D2D1_MATRIX_3X2_F oldTransform;
        D2D::context->GetTransform(&oldTransform);

        if (CursorInRect(x, y, width, height)) {

            FlarialGUI::lerp(rotationAngles[index], rotationAngles[index] + 15, 0.5f * FlarialGUI::frameFactor);
        }


        float rotationAngle = rotationAngles[index];// Specify the rotation angle in degrees
        D2D1_POINT_2F rotationCenter = D2D1::Point2F(imagerectf.left + imageWidth / 2.0f, imagerectf.top +
                                                                                          imageHeight /
                                                                                          2.0f);  // Specify the rotation center
        D2D1_MATRIX_3X2_F rotationMatrix = D2D1::Matrix3x2F::Rotation(rotationAngle, rotationCenter);

        D2D1_MATRIX_3X2_F translationMatrix = D2D1::Matrix3x2F::Translation(x, imageY);
        D2D1_MATRIX_3X2_F combinedMatrix = translationMatrix * rotationMatrix;

        D2D::context->SetTransform(combinedMatrix);

        imagerectf = D2D1::RectF(0, 0, imageWidth, imageHeight);

        D2D::context->DrawBitmap(ImagesClass::images[iconId].get(), imagerectf, 1.0, D2D1_INTERPOLATION_MODE_ANISOTROPIC);

        D2D::context->SetTransform(oldTransform);
    }
}