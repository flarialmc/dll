#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../Structs/HSV.hpp"

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

void FlarialGUI::ColorPickerWindow(int index, std::string& hex, float& opacity, bool& rgb) {
	if (ColorPickers[index].isActive) {
		// 75% opacity black rect
		FlarialGUI::RoundedRect(0, 0, D2D1::ColorF(D2D1::ColorF::Black, 0.75),
			Constraints::RelativeConstraint(1.5, "width", true),
			Constraints::RelativeConstraint(1.5, "height", true), 0, 0);

		float rectwidth = Constraints::RelativeConstraint(0.55, "height", true);
		float rectheight = Constraints::RelativeConstraint(0.45, "height", true);
		Vec2<float> center = Constraints::CenterConstraint(rectwidth, rectheight);
		Vec2<float> round = Constraints::RoundingConstraint(45, 45);

		D2D1_COLOR_F colorThing = colors_secondary2_rgb ? rgbColor : colors_secondary2;
		colorThing.a = o_colors_secondary2;

		D2D1_COLOR_F anotherColor = colors_secondary1_rgb ? rgbColor : colors_secondary1;
		anotherColor.a = o_colors_secondary1;

		D2D1_COLOR_F textCol = colors_text_rgb ? rgbColor : colors_text;
		textCol.a = o_colors_text;

		FlarialGUI::RoundedHollowRect(center.x, center.y, Constraints::RelativeConstraint(0.01, "height", true),
			colorThing, rectwidth, rectheight, round.x, round.x);
		FlarialGUI::RoundedRect(center.x, center.y, anotherColor, rectwidth, rectheight, round.x, round.x);

		FlarialGUI::PushSize(center.x, center.y, rectwidth, rectheight);

		float x = Constraints::PercentageConstraint(0.07, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");
		float spacing = Constraints::SpacingConstraint(0.15, rectheight);
		float hexPreviewSize = Constraints::SpacingConstraint(0.265, rectheight);

		// hue line and opacity line
		float hlwidth = Constraints::SpacingConstraint(0.85f, rectwidth);
		float gap2L = Constraints::SpacingConstraint(0.3f, hexPreviewSize);
		float shadePickerHeight = hexPreviewSize * 2.0f + Constraints::SpacingConstraint(0.1, hexPreviewSize);
		float shadePickerWidth = rectwidth - (hexPreviewSize * 1.86f);

		HSV hsv_color{};

		if (ColorPickers[index].shade.x == -1 && ColorPickers[index].shade.y == -1) {
			hsv_color = RGBtoHSV(HexToColorF(hex));

			ColorPickers[index].hueX = (hsv_color.hue / 360.0f) * hlwidth;
			ColorPickers[index].oldHueX = (hsv_color.hue / 360.0f) * hlwidth;
			ColorPickers[index].shade.x = hsv_color.saturation * shadePickerWidth;
			ColorPickers[index].shade.y = (1.0f - hsv_color.value) * shadePickerHeight;
		}

		D2D1_COLOR_F color = HSVtoColorF(
			(ColorPickers[index].hueX / hlwidth) * 360,
			ColorPickers[index].shade.x / shadePickerWidth,
			1.0f - ColorPickers[index].shade.y / shadePickerHeight
		);

		color.a = ColorPickers[index].opacX / hlwidth;
		ColorPickers[index].opacX = opacity * hlwidth;

		if (ColorPickers[index].oldHex.empty()) ColorPickers[index].oldHex = hex;
		if (ColorPickers[index].oldOpac == NULL) ColorPickers[index].oldOpac = opacity;

		D2D1_COLOR_F oldColor = HexToColorF(ColorPickers[index].oldHex);
		oldColor.a = ColorPickers[index].oldOpac;

		// color preview square

		FlarialGUI::image(IDR_TRANSPARENT_PNG, D2D1::RectF(
			x + 1,
			y + 1,
			x + hexPreviewSize - 1,
			y - 1 + hexPreviewSize - 1
		));
		RoundedRect(x, y, color, hexPreviewSize, hexPreviewSize, 0, 0);

		// previous color preview square

		FlarialGUI::image(IDR_TRANSPARENT_PNG, D2D1::RectF(
			x + 1,
			y + 1 + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize),
			x + hexPreviewSize - 1,
			y - 1 + hexPreviewSize * 2 + Constraints::SpacingConstraint(0.1, hexPreviewSize) - 1
		));
		RoundedRect(x, y + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize), oldColor,
			hexPreviewSize, hexPreviewSize, 0, 0);

		ImColor colors[] = {
			ImColor(255, 0, 0),
			ImColor(255, 255, 0),
			ImColor(0, 255, 0),
			ImColor(0, 255, 255),
			ImColor(0, 0, 255),
			ImColor(255, 0, 255),
			ImColor(255, 0, 0)
		};

		/* HUE SELECTOR */
		for (int i = 0; i < 6; ++i)
		{
			ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(
				ImVec2(x + i * (hlwidth / 6), y + hexPreviewSize * 2 + Constraints::SpacingConstraint(0.3f, hexPreviewSize)),
				ImVec2(x + (i + 1) * (hlwidth / 6), y + hexPreviewSize * 2 + Constraints::SpacingConstraint(0.4f, hexPreviewSize)),
				colors[i],
				colors[i + 1],
				colors[i + 1],
				colors[i]
			);
		}
		/* HUE SELECTOR */

		/* OPACITY */
		float currentHue = (ColorPickers[index].hueX / hlwidth) * 360.0f;

		D2D1_COLOR_F lol = HSVtoColorF(currentHue, 1.0f, 1.0f);
		ImColor hueColor = ImColor(lol.r, lol.g, lol.g, lol.a);

		ImU32 col_left  = IM_COL32((int)(hueColor.Value.x * 255), (int)(hueColor.Value.y * 255), (int)(hueColor.Value.z * 255), 0);
		ImU32 col_right = IM_COL32((int)(hueColor.Value.x * 255), (int)(hueColor.Value.y * 255), (int)(hueColor.Value.z * 255), 255);

		ImVec2 rect_min(x, y + hexPreviewSize * 2.31 + Constraints::SpacingConstraint(0.3f, hexPreviewSize));
		ImVec2 rect_max(x + hlwidth, y + hexPreviewSize * 2.31 + Constraints::SpacingConstraint(0.4f, hexPreviewSize));


		ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(
			rect_min,
			rect_max,
			col_left,
			col_right,
			col_right,
			col_left
		);
		/* OPACITY */


		y = Constraints::PercentageConstraint(0.10, "top");
		float originalY = y;

		// COOLOR PICKER ALLCOLORS
		ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(ImVec2(x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize), y), ImVec2(x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize) + shadePickerWidth, y + shadePickerHeight), IM_COL32_WHITE, ImColor(ImColor::HSV(ColorPickers[index].hueX / hlwidth, 1.0f, 1.0f)), ImColor(ImColor::HSV(ColorPickers[index].hueX / hlwidth, 1.0f, 1.0f)), IM_COL32_WHITE);
		ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(ImVec2(x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize), y), ImVec2(x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize) + shadePickerWidth, y + shadePickerHeight), 0, 0, IM_COL32_BLACK, IM_COL32_BLACK);
		// COLOR PICKER ALL COLORS

		y = Constraints::PercentageConstraint(0.10, "top");

		D2D1_COLOR_F hueSelectorerOutline = colors_primary2_rgb ? rgbColor : colors_primary2;
		hueSelectorerOutline.a = o_colors_primary2;

		float circleX = x + ColorPickers[index].oldHueX;
		float circleY = y + hexPreviewSize * 2 + Constraints::SpacingConstraint(0.35f, hexPreviewSize);

		Circle(circleX, circleY, hueSelectorerOutline, Constraints::SpacingConstraint(0.125f, hexPreviewSize));
		Circle(circleX, circleY, HSVtoColorF(
			(ColorPickers[index].oldHueX / hlwidth) * 360,
			1.0f,
			1.0f
		), Constraints::SpacingConstraint(0.08f, hexPreviewSize));

		if (
			((Utils::CursorInEllipse(circleX, circleY, Constraints::SpacingConstraint(0.15f, hexPreviewSize),
				Constraints::SpacingConstraint(0.15f, hexPreviewSize)) &&
				MC::held) || (ColorPickers[index].movingHueX && MC::held) ||
				(MC::held && CursorInRect(
					x,
					y + hexPreviewSize * 2 + Constraints::SpacingConstraint(0.3f, hexPreviewSize),
					hlwidth,
					Constraints::SpacingConstraint(0.1f, hexPreviewSize)))) &&
			!ColorPickers[index].movingOpacX && !ColorPickers[index].movingShade
			) {
			ColorPickers[index].oldHueX =
				MC::mousePos.x > (x + hlwidth) ? hlwidth : (MC::mousePos.x < x ? 0.0f : MC::mousePos.x - x);
			ColorPickers[index].movingHueX = true;
		}
		else {
			ColorPickers[index].movingHueX = false;
			ColorPickers[index].hueX = ColorPickers[index].oldHueX;
		}

		y += gap2L;
		circleY += gap2L;
		circleX = x + ColorPickers[index].opacX;

		Circle(circleX, circleY, hueSelectorerOutline, Constraints::SpacingConstraint(0.125f, hexPreviewSize));

		if (
			((Utils::CursorInEllipse(circleX, circleY, Constraints::SpacingConstraint(0.15f, hexPreviewSize),
				Constraints::SpacingConstraint(0.15f, hexPreviewSize)) &&
				MC::held) || (ColorPickers[index].movingOpacX && MC::held) ||
				(MC::held && CursorInRect(
					x,
					y + hexPreviewSize * 2 + Constraints::SpacingConstraint(0.3f, hexPreviewSize),
					hlwidth,
					Constraints::SpacingConstraint(0.1f, hexPreviewSize)))) &&
			!ColorPickers[index].movingHueX && !ColorPickers[index].movingShade
			) {
			ColorPickers[index].opacX =
				MC::mousePos.x > (x + hlwidth) ? hlwidth : (MC::mousePos.x < x ? 0.0f : MC::mousePos.x - x);
			ColorPickers[index].movingOpacX = true;
		}
		else ColorPickers[index].movingOpacX = false;

		y -= gap2L;

		float originalX = x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize);

		if ((CursorInRect(
			x + hexPreviewSize + Constraints::SpacingConstraint(0.1, hexPreviewSize),
			y,
			shadePickerWidth,
			shadePickerHeight
		) || ColorPickers[index].movingShade) && MC::held && !ColorPickers[index].movingOpacX &&
			!ColorPickers[index].movingHueX) {
			ColorPickers[index].shade.x =
				MC::mousePos.x > (originalX + shadePickerWidth) ? shadePickerWidth : (MC::mousePos.x < originalX ? 0
					:
					MC::mousePos.x - originalX);
			ColorPickers[index].shade.y =
				MC::mousePos.y > (originalY + shadePickerHeight) ? shadePickerHeight : (MC::mousePos.y < originalY
					? 0 : MC::mousePos.y -
					originalY);
			ColorPickers[index].movingShade = true;
		}
		else ColorPickers[index].movingShade = false;

		D2D1_COLOR_F newColorLol = HSVtoColorF(
			(ColorPickers[index].hueX / hlwidth) * 360,
			ColorPickers[index].shade.x / shadePickerWidth,
			1.0f - ColorPickers[index].shade.y / shadePickerHeight
		);

		hex = ColorFToHex(newColorLol);
		opacity = ColorPickers[index].opacX / hlwidth;

		Circle(ColorPickers[index].shade.x + originalX, ColorPickers[index].shade.y + originalY, hueSelectorerOutline,
			Constraints::SpacingConstraint(0.125f, hexPreviewSize));
		Circle(ColorPickers[index].shade.x + originalX, ColorPickers[index].shade.y + originalY, newColorLol,
			Constraints::SpacingConstraint(0.08f, hexPreviewSize));

		y += spacing * 4.95f;

		float buttonWidth = Constraints::RelativeConstraint(0.25f, "width");
		float buttonHeight = Constraints::RelativeConstraint(0.13f, "height");
		if (RoundedButton(0, Constraints::PercentageConstraint(0.07, "right") - buttonWidth, y, colorThing, textCol,
			L"Close", buttonWidth, buttonHeight, round.x, round.x)) {
			ColorPickers[index].isActive = false;
			ColorPickers[index].oldHex = ColorFToHex(newColorLol);
			ColorPickers[index].oldOpac = ColorPickers[index].opacX / hlwidth;
			activeColorPickerWindows--;
		}

		y += spacing * 0.3f;

		x = Constraints::PercentageConstraint(0.04, "left");

		if (Toggle(123, x, y, rgb, true)) rgb = !rgb;

		FlarialTextWithFont(
			x + Constraints::SpacingConstraint(0.60, Constraints::RelativeConstraint(0.12, "height", true)), y,
			L"Chroma",
			Constraints::RelativeConstraint(0.12, "height", true) * 3.0f,
			Constraints::RelativeConstraint(0.029, "height", true), DWRITE_TEXT_ALIGNMENT_LEADING,
			Constraints::RelativeConstraint(0.12, "height", true),
			DWRITE_FONT_WEIGHT_NORMAL);
		

		/*
		for (int j = 0; j < hlwidth - 1; ++j) {
			ID2D1LinearGradientBrush* gbrush;

			D2D::context->FillRectangle(
					D2D1::RectF(
							x + static_cast<FLOAT>(j),
							y + hlheight + hexPreviewSize,
							x + static_cast<FLOAT>(j + 1),
							y + hlheight + hexPreviewSize + static_cast<FLOAT>(hlheight)
							),
					gbrush
			);
		}
		 */

		 /*
		 FlarialGUI::FlarialTextWithFont(x + spacing, y - spacing * 1.45, L"Color Sliders",
										 Constraints::RelativeConstraint(1, "width"),
										 Constraints::RelativeConstraint(0.2, "width"), DWRITE_TEXT_ALIGNMENT_JUSTIFIED,
										 Constraints::RelativeConstraint(0.5, "width"), DWRITE_FONT_WEIGHT_NORMAL);

		 spacing *= 0.90f;

		 float percentR = FlarialGUI::Slider(index + 10, x, y, color.r * 255.0f, 255.0f, 0, 0);

		 float percentG = FlarialGUI::Slider(index + 11, x, y + spacing, color.g * 255.0f, 255.0f, 0, 0);

		 float percentB = FlarialGUI::Slider(index + 12, x, y + spacing * 2, color.b * 255.0f, 255.0f, 0, 0);

		 opacity = FlarialGUI::Slider(index + 200, x, y + spacing * 3, opacity, 1.0, 0, 0);

		 float buttonWidth = Constraints::RelativeConstraint(0.35f, "width");
		 float buttonHeight = Constraints::RelativeConstraint(0.20f, "height");
		 if (FlarialGUI::RoundedButton(0, x + spacing * 1.45f, y + spacing * 4.12f, D2D1::ColorF(32.0f / 255.0f, 26.0f / 255.0f, 27.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), L"Close", buttonWidth, buttonHeight, round.x, round.x)) ColorPickers[index].isActive = false;



		 color.r = percentR / 255.0f;
		 color.g = percentG / 255.0f;
		 color.b = percentB / 255.0f;

		 hex = FlarialGUI::ColorFToHex(color);
		 */

		FlarialGUI::PopSize();

		FlarialGUI::UnsetWindowRect();

		// rect containing shit

		// saves this hex to ColorPickers[index].oldHex

		// have 4 sliders for R G B A (converts hex -> d2d1colorf, sets the rgba sliders starting point accordingly)

		// e.g. float r = Slider(...)

		// converts to hex, saves to ColorPickers[index].newHex

		// big rect that show the current hex color (newHex)

		// revert button (reverts to oldHex)

		// save button (converts rgba -> hex, saves to &hex)

	}
}