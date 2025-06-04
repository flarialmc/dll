#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../Structs/ImagesClass.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../../Assets/Assets.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

std::string FlarialGUI::Dropdown(int index, float x, float y, const std::vector<std::string>& options, std::string &value, const std::string& label, std::string moduleName, std::string settingName) {

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

    D2D1_COLOR_F unselectedChildCol = clickgui->getColor("primary3", "ClickGUI");
    D2D1_COLOR_F selectedCol = clickgui->getColor("primary1", "ClickGUI");
    D2D1_COLOR_F hoveredChildCol = clickgui->getColor("primary4", "ClickGUI");

    if (ClickGUI::settingsOpacity != 1) {
        unselectedChildCol.a = ClickGUI::settingsOpacity;
        selectedCol.a = ClickGUI::settingsOpacity;
        hoveredChildCol.a = ClickGUI::settingsOpacity;
    }

    unselectedChildCol.a = clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
    selectedCol.a = clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
    hoveredChildCol.a = clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;

    float originalY = y;
    if (!FlarialGUI::DropDownMenus[index].curColorDone) {
        FlarialGUI::DropDownMenus[index].curColor = unselectedChildCol;
        y = y - maxHeight;
        FlarialGUI::DropDownMenus[index].yChilds = y;
        FlarialGUI::DropDownMenus[index].curColorDone = true;
    } else y = FlarialGUI::DropDownMenus[index].yChilds;

    

    if (CursorInRect(x, clickingY, Constraints::SpacingConstraint(1.85, textWidth), percHeight + maxHeight) && FlarialGUI::DropDownMenus[index].isActive && MC::mouseButton == MouseButton::Right && !MC::held) {
        if (Client::settings.getSettingByName<bool>("resettableSettings")->value && moduleName != "" && settingName != "") {
		    auto mod = ModuleManager::getModule(moduleName);
			mod->settings.deleteSetting(settingName);
			mod->defaultConfig();
			value = mod->settings.getSettingByName<std::string>(settingName)->value;
		}
    }

    if (!activeColorPickerWindows &&
        CursorInRect(x, clickingY, Constraints::SpacingConstraint(1.85, textWidth), percHeight + maxHeight)) {
        if (MC::mouseButton == MouseButton::Left &&
            CursorInRect(x, clickingY, Constraints::SpacingConstraint(1.85, textWidth), percHeight)) {
            //MC::mouseButton = MouseButton::None;
            FlarialGUI::DropDownMenus[index].isActive = true;
            value = FlarialGUI::DropDownMenus[index].selected;
        }
    } else if (!CursorInRect(x, clickingY, Constraints::SpacingConstraint(1.85, textWidth), percHeight + maxHeight)) {
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
                FlarialGUI::RoundedRect(x + offset, curY, unselectedChildCol, Constraints::SpacingConstraint(1.85,
                                                                                                             curTextWidth),
                                        childHeights, 0, 0);

            counter++;
            curY = y + ((float)counter * childHeights) - ((float)counter * 0.1f) + 5.f;
        }

        if(!FlarialGUI::DropDownMenus[index].isActive) break;

        if (curY < (originalY - lastChildHeight + 20)) continue;

        float curClickingY = clickingY + ((float)counter * childHeights) + 5.f;

        if (counter == options.size() - 1) {
            FlarialGUI::RoundedRect(x + offset, curY, unselectedChildCol,
                                    Constraints::SpacingConstraint(1.85, curTextWidth), lastChildHeight, 0, 0);
            FlarialGUI::RoundedRect(x + offset, curY, unselectedChildCol,
                                    Constraints::SpacingConstraint(1.85, curTextWidth), childHeights, round.x, round.y);
        } else
            FlarialGUI::RoundedRect(x + offset, curY, unselectedChildCol,
                                    Constraints::SpacingConstraint(1.85, curTextWidth), childHeights, 0, 0);

        if (CursorInRect(x + offset, curClickingY, Constraints::SpacingConstraint(1.85, curTextWidth), childHeights)) {
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
                                    Constraints::SpacingConstraint(1.85, curTextWidth), lastChildHeight, 0, 0);
            FlarialGUI::RoundedRect(x + offset, FlarialGUI::DropDownMenus[index].yHover, hoveredChildCol,
                                    Constraints::SpacingConstraint(1.85, curTextWidth), childHeights, round.x, round.y);
        } else {
            FlarialGUI::RoundedRect(x + offset, FlarialGUI::DropDownMenus[index].yHover, hoveredChildCol,
                                    Constraints::SpacingConstraint(1.85, curTextWidth), childHeights, 0, 0);
        }
    }

    counter = 1;

    for (const std::string &op: options) {
        if (op == FlarialGUI::DropDownMenus[index].selected) continue;

        float curY = y + ((float)counter * childHeights) - ((float)counter * 0.1f) + 5.f;

        if (curY < originalY) continue;

        if(op != FlarialGUI::DropDownMenus[index].selected && !FlarialGUI::DropDownMenus[index].isActive) break;


        FlarialGUI::FlarialTextWithFont(x + offset + Constraints::SpacingConstraint(0.1, curTextWidth),
                                        counter == options.size() - 1 ? curY - 2.5f : curY - 4.0f, to_wide(op).c_str(),
                                        Constraints::SpacingConstraint(1.85, curTextWidth), percHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.0, curTextWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        counter++;
    }

    // if (FlarialGUI::DropDownMenus[index].isActive) MC::mouseButton = MouseButton::None;

    y = originalY;

    FlarialGUI::RoundedRect(x, y, FlarialGUI::DropDownMenus[index].curColor,
                            Constraints::SpacingConstraint(1.85, textWidth), percHeight, round.x, round.x);

    FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.1, textWidth), y,
                                    FlarialGUI::to_wide(value).c_str(),
                                    Constraints::SpacingConstraint(1.85, textWidth), percHeight,
                                    DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.0, textWidth),
                                    DWRITE_FONT_WEIGHT_NORMAL);
    FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.70, textWidth), y,
                                    FlarialGUI::to_wide(label).c_str(),
                                    Constraints::SpacingConstraint(3, textWidth), percHeight,
                                    DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.00, textWidth),
                                    DWRITE_FONT_WEIGHT_NORMAL);

    float is = percHeight / 2;
    float ix = x + Constraints::SpacingConstraint(1.8, textWidth) - is * 1.2f;
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