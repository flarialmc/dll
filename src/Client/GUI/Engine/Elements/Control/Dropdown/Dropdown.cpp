#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../Structs/ImagesClass.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../../Assets/Assets.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_text HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_text")->value)
#define o_colors_text clickgui->settings.getSettingByName<float>("o_colors_text")->value
#define colors_text_rgb clickgui->settings.getSettingByName<bool>("colors_text_rgb")->value

#define colors_primary1 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary1")->value)
#define o_colors_primary1 clickgui->settings.getSettingByName<float>("o_colors_primary1")->value
#define colors_primary1_rgb clickgui->settings.getSettingByName<bool>("colors_primary1_rgb")->value

#define colors_primary2 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary2")->value)
#define o_colors_primary2 clickgui->settings.getSettingByName<float>("o_colors_primary2")->value
#define colors_primary2_rgb clickgui->settings.getSettingByName<bool>("colors_primary2_rgb")->value

#define colors_primary3 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary3")->value)
#define o_colors_primary3 clickgui->settings.getSettingByName<float>("o_colors_primary3")->value
#define colors_primary3_rgb clickgui->settings.getSettingByName<bool>("colors_primary3_rgb")->value

#define colors_primary4 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary4")->value)
#define o_colors_primary4 clickgui->settings.getSettingByName<float>("o_colors_primary4")->value
#define colors_primary4_rgb clickgui->settings.getSettingByName<bool>("colors_primary4_rgb")->value

#define colors_secondary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary1")->value)
#define o_colors_secondary1 clickgui->settings.getSettingByName<float>("o_colors_secondary1")->value
#define colors_secondary1_rgb clickgui->settings.getSettingByName<bool>("colors_secondary1_rgb")->value

#define colors_secondary2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary2")->value)
#define o_colors_secondary2 clickgui->settings.getSettingByName<float>("o_colors_secondary2")->value
#define colors_secondary2_rgb clickgui->settings.getSettingByName<bool>("colors_secondary2_rgb")->value

#define colors_secondary7 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary7")->value)
#define o_colors_secondary7 clickgui->settings.getSettingByName<float>("o_colors_secondary7")->value
#define colors_secondary7_rgb clickgui->settings.getSettingByName<bool>("colors_secondary7_rgb")->value

std::string FlarialGUI::Dropdown(int index, float x, float y, const std::vector<std::string>& options, std::string &value,
                     const std::string& label) {

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);
    const bool isAdditionalY = shouldAdditionalY;
    const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
    const float percHeight = Constraints::RelativeConstraint(0.035, "height", true);

    y -= percHeight / 2.0f;

    float childHeights = Constraints::RelativeConstraint(0.030, "height", true);
    float maxHeight = ((float)options.size() - 1.0f) * childHeights + 2.0f;
    float addYVal = maxHeight + Constraints::SpacingConstraint(0.05, textWidth);
    float clickingY = y;

    if (isInScrollView) clickingY += scrollpos;

    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i != index && additionalY[i] > 0.0f) {
                y += additionalY[i];
            }
        }
    }


    if (index >= highestAddIndexes) {
        highestAddIndexes = index;
    }
    additionalIndex = index;

    D2D1_COLOR_F unselectedChildCol = colors_primary3_rgb ? rgbColor : colors_primary3;
    unselectedChildCol.a = ClickGUI::settingsOpacity;


    D2D1_COLOR_F selectedCol = colors_primary1_rgb ? rgbColor : colors_primary1;
    selectedCol.a = ClickGUI::settingsOpacity;

    float originalY = y;
    if (!FlarialGUI::DropDownMenus[index].curColorDone) {
        FlarialGUI::DropDownMenus[index].curColor = unselectedChildCol;
        y = y - maxHeight;
        FlarialGUI::DropDownMenus[index].yChilds = y;
        FlarialGUI::DropDownMenus[index].curColorDone = true;
    } else y = FlarialGUI::DropDownMenus[index].yChilds;

    D2D1_COLOR_F hoveredChildCol = colors_primary4_rgb ? rgbColor : colors_primary4;
    hoveredChildCol.a = o_colors_primary4;

    if (!activeColorPickerWindows &&
        CursorInRect(x, clickingY, Constraints::SpacingConstraint(1.55, textWidth), percHeight + maxHeight)) {
        if (MC::mouseButton == MouseButton::Left &&
            CursorInRect(x, clickingY, Constraints::SpacingConstraint(1.55, textWidth), percHeight)) {
            //MC::mouseButton = MouseButton::None;
            FlarialGUI::DropDownMenus[index].isActive = true;
            value = FlarialGUI::DropDownMenus[index].selected;
        }
    } else if (!CursorInRect(x, clickingY, Constraints::SpacingConstraint(1.55, textWidth), percHeight + maxHeight)) {
        if (MC::mouseButton == MouseButton::Left) {
            //MC::mouseButton = MouseButton::None;
            FlarialGUI::DropDownMenus[index].isActive = false;
            value = FlarialGUI::DropDownMenus[index].selected;
        }
        FlarialGUI::lerp(FlarialGUI::DropDownMenus[index].opacityHover, 0.0f, 0.25f * FlarialGUI::frameFactor);
    } else if (!FlarialGUI::DropDownMenus[index].isActive) {
        if (FlarialGUI::DropDownMenus[index].firstTime) {
            FlarialGUI::DropDownMenus[index].selected = value;
            FlarialGUI::DropDownMenus[index].firstTime = false;
        }
        value = FlarialGUI::DropDownMenus[index].selected;
    }

    if (FlarialGUI::DropDownMenus[index].isActive) {
        //y = originalY;
        if(additionalY[additionalIndex] < 0) additionalY[additionalIndex] = 0;


        FlarialGUI::lerp(additionalY[additionalIndex], addYVal, 0.25f * FlarialGUI::frameFactor);

        FlarialGUI::lerp(y, originalY, 0.25f * FlarialGUI::frameFactor);
        FlarialGUI::DropDownMenus[index].curColor = FlarialGUI::LerpColor(FlarialGUI::DropDownMenus[index].curColor,
                                                                          selectedCol, 0.1f * FlarialGUI::frameFactor);

        FlarialGUI::lerp(
                FlarialGUI::DropDownMenus[index].rotation,
                270.f,
                0.25f * FlarialGUI::frameFactor
        );
    } else {
        //y = originalY - maxHeight;

        FlarialGUI::lerp(additionalY[additionalIndex], -50.f, 0.1f * FlarialGUI::frameFactor);

        FlarialGUI::lerp(y, originalY - maxHeight, 0.25f * FlarialGUI::frameFactor);
        FlarialGUI::DropDownMenus[index].curColor = FlarialGUI::LerpColor(FlarialGUI::DropDownMenus[index].curColor,
                                                                          unselectedChildCol,
                                                                          0.1f * FlarialGUI::frameFactor);
        FlarialGUI::lerp(
                FlarialGUI::DropDownMenus[index].rotation,
                90.f,
                0.25f * FlarialGUI::frameFactor
        );
    }

    if (!FlarialGUI::DropDownMenus[index].hoveredIndex) FlarialGUI::DropDownMenus[index].hoveredIndex = 1;

    FlarialGUI::DropDownMenus[index].yChilds = y;

    int counter = 0;

    float curTextWidth = Constraints::RelativeConstraint(0.1175, "height", true);
    float offset = textWidth - curTextWidth - 1;
    float lastChildHeight = childHeights - 8;

    if (isAdditionalY) UnSetIsInAdditionalYMode();

    for (const std::string &op: options) {
        if (op == FlarialGUI::DropDownMenus[index].selected) continue;

        float curY = y + ((float)counter * childHeights) - ((float)counter * 0.1f) + 5.f;

        if (counter == 0) {
            if (curY > (originalY - lastChildHeight + 20))
                FlarialGUI::RoundedRect(x + offset, curY, unselectedChildCol, Constraints::SpacingConstraint(1.55,
                                                                                                             curTextWidth),
                                        childHeights, 0, 0);

            counter++;
            curY = y + ((float)counter * childHeights) - ((float)counter * 0.1f) + 5.f;
        }

        if (curY < (originalY - lastChildHeight + 20)) continue;

        float curClickingY = clickingY + ((float)counter * childHeights) + 5.f;

        if (counter == options.size() - 1) {
            FlarialGUI::RoundedRect(x + offset, curY, unselectedChildCol,
                                    Constraints::SpacingConstraint(1.55, curTextWidth), lastChildHeight, 0, 0);
            FlarialGUI::RoundedRect(x + offset, curY, unselectedChildCol,
                                    Constraints::SpacingConstraint(1.55, curTextWidth), childHeights, round.x, round.y);
        } else
            FlarialGUI::RoundedRect(x + offset, curY, unselectedChildCol,
                                    Constraints::SpacingConstraint(1.55, curTextWidth), childHeights, 0, 0);

        if (CursorInRect(x + offset, curClickingY, Constraints::SpacingConstraint(1.55, curTextWidth), childHeights)) {
            if (MC::mouseButton == MouseButton::Left && FlarialGUI::DropDownMenus[index].isActive) {
                MC::mouseButton = MouseButton::None;
                FlarialGUI::DropDownMenus[index].isActive = false;
                FlarialGUI::DropDownMenus[index].opacityHover = 0.0f;
                value = op;
            } else {
                FlarialGUI::lerp(FlarialGUI::DropDownMenus[index].opacityHover, 1.0f, 0.25f * FlarialGUI::frameFactor);
            }
            FlarialGUI::DropDownMenus[index].hoveredIndex = counter;
        }
        counter++;
    }

    FlarialGUI::lerp(FlarialGUI::DropDownMenus[index].yHover,
                     y + ((float)FlarialGUI::DropDownMenus[index].hoveredIndex * childHeights) - ((float)counter * 0.1f) + 5.f,
                     0.25f * FlarialGUI::frameFactor);

    if (FlarialGUI::DropDownMenus[index].yHover >= originalY &&
        (originalY + percHeight + maxHeight) >= FlarialGUI::DropDownMenus[index].yHover) {
        hoveredChildCol.a = FlarialGUI::DropDownMenus[index].opacityHover;
        if (FlarialGUI::DropDownMenus[index].hoveredIndex == options.size() - 1) {
            FlarialGUI::RoundedRect(x + offset, FlarialGUI::DropDownMenus[index].yHover, hoveredChildCol,
                                    Constraints::SpacingConstraint(1.55, curTextWidth), lastChildHeight, 0, 0);
            FlarialGUI::RoundedRect(x + offset, FlarialGUI::DropDownMenus[index].yHover, hoveredChildCol,
                                    Constraints::SpacingConstraint(1.55, curTextWidth), childHeights, round.x, round.y);
        } else {
            FlarialGUI::RoundedRect(x + offset, FlarialGUI::DropDownMenus[index].yHover, hoveredChildCol,
                                    Constraints::SpacingConstraint(1.55, curTextWidth), childHeights, 0, 0);
        }
    }

    counter = 1;

    for (const std::string &op: options) {
        if (op == FlarialGUI::DropDownMenus[index].selected) continue;

        float curY = y + ((float)counter * childHeights) - ((float)counter * 0.1f) + 5.f;

        if (curY < originalY) continue;

        FlarialGUI::FlarialTextWithFont(x + offset + Constraints::SpacingConstraint(0.1, curTextWidth),
                                        counter == options.size() - 1 ? curY - 2.5f : curY - 4.0f, to_wide(op).c_str(),
                                        Constraints::SpacingConstraint(1.55, curTextWidth), percHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.0, curTextWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        counter++;
    }

    // if (FlarialGUI::DropDownMenus[index].isActive) MC::mouseButton = MouseButton::None;

    y = originalY;

    FlarialGUI::RoundedRect(x, y, FlarialGUI::DropDownMenus[index].curColor,
                            Constraints::SpacingConstraint(1.55, textWidth), percHeight, round.x, round.x);

    FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.1, textWidth), y,
                                    FlarialGUI::to_wide(value).c_str(),
                                    Constraints::SpacingConstraint(1.55, textWidth), percHeight,
                                    DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.0, textWidth),
                                    DWRITE_FONT_WEIGHT_NORMAL);
    FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.70, textWidth), y,
                                    FlarialGUI::to_wide(label).c_str(),
                                    Constraints::SpacingConstraint(3, textWidth), percHeight,
                                    DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.00, textWidth),
                                    DWRITE_FONT_WEIGHT_NORMAL);

    float is = percHeight / 2;
    float ix = x + Constraints::SpacingConstraint(1.5, textWidth) - is * 1.2f;
    float iy = y + Constraints::SpacingConstraint(0.28, percHeight);

    if (ImagesClass::images[IDR_DOWN_PNG] == nullptr)
        LoadImageFromResource(IDR_DOWN_PNG, &ImagesClass::images[IDR_DOWN_PNG]);

    /*
    D2D1_MATRIX_3X2_F oldTransform;
    D2D::context->GetTransform(&oldTransform);
    */

    /*

    D2D1_POINT_2F rotationCenter = D2D1::Point2F(rectf.left + is / 2, rectf.top + is / 2);
    D2D1_MATRIX_3X2_F rotationMatrix = D2D1::Matrix3x2F::Rotation(FlarialGUI::DropDownMenus[index].rotation,
                                                                  rotationCenter);

    D2D::context->SetTransform(rotationMatrix);

    D2D::context->DrawBitmap(ImagesClass::images[IDR_DOWN_PNG], rectf, 1.0f, D2D1_INTERPOLATION_MODE_ANISOTROPIC);

    D2D::context->SetTransform(oldTransform);
    */

    if (FlarialGUI::isInScrollView) {
        iy += FlarialGUI::scrollpos;
    }
    auto rectf = D2D1::RectF(ix, iy, ix + is, iy + is);

    float rotationAngle = FlarialGUI::DropDownMenus[index].rotation;
    ImVec2 rotationCenter(ix + is / 2, iy + is / 2);

    FlarialGUI::ImRotateStart();
    FlarialGUI::image(IDR_DOWN_PNG, rectf, "PNG", false);
    FlarialGUI::ImRotateEnd(rotationAngle, rotationCenter);


    if (FlarialGUI::isInScrollView) {
        iy -= FlarialGUI::scrollpos;
    }

    FlarialGUI::RoundedRect(ix - 8, iy - 5,
                            FlarialGUI::DropDownMenus[index].isActive ? D2D1::ColorF(D2D1::ColorF::White)
                                                                      : D2D1::ColorF(192.0f / 255.0f, 133.0f / 255.0f,
                                                                                     142.0f / 255.0f), 1, is + 8, 0, 0);

    FlarialGUI::DropDownMenus[index].selected = value;

    if (isAdditionalY) SetIsInAdditionalYMode();

    return FlarialGUI::DropDownMenus[index].selected;
}