#include "../../../Engine.hpp"
#include "../../../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

std::vector<float> searchBarSizes;
std::vector<float> searchCutOutHeights;
static std::string name = "";

std::string ClickGUIElements::SearchBar(int index, std::string& text, int limit, float x, float y) {

	if (ClickGUI::page.type == "normal" && ClickGUI::curr == "modules") {

		D2D1_COLOR_F col;

		Vec2<float> round = Constraints::RoundingConstraint(19, 19);

		if (index > searchBarSizes.size() - 1 || index == 0) {
			float nigga = Constraints::RelativeConstraint(0.42, "height");

			searchBarSizes.emplace_back(nigga);
		}

		if (index > searchCutOutHeights.size() - 1 || index == 0) {
			float nigga = Constraints::RelativeConstraint(0.38, "height");

			searchCutOutHeights.emplace_back(nigga);
		}

		if (FlarialGUI::TextBoxes[index].isActive) {
			if (FlarialGUI::TextBoxes[index].isAt1)
				FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, -1.0f, 0.05f * FlarialGUI::frameFactor);
			else FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, 2.0f, 0.05f * FlarialGUI::frameFactor);
		}
		else FlarialGUI::TextBoxes[index].cursorOpac = 0;

		if (FlarialGUI::TextBoxes[index].cursorOpac > 1) FlarialGUI::TextBoxes[index].isAt1 = true;
		if (FlarialGUI::TextBoxes[index].cursorOpac < 0) FlarialGUI::TextBoxes[index].isAt1 = false;

		if (index <= searchBarSizes.size()) {

			const float textWidth = searchBarSizes[index];
			const float percHeight = Constraints::RelativeConstraint(0.42, "height");

			text = FlarialGUI::TextBox(index, text, limit, x - textWidth, y, textWidth, percHeight, 3);

			if (!text.empty()) FlarialGUI::TextBoxes[index].isActive = true;

			if (FlarialGUI::CursorInRect(x - textWidth, y, textWidth, percHeight) && MC::mouseButton == MouseButton::Right && !MC::held) {
				FlarialGUI::TextBoxes[index].text = "";
			}

			if (FlarialGUI::TextBoxes[index].isActive) {

				FlarialGUI::lerp(searchBarSizes[index], Constraints::RelativeConstraint(2.7f, "height"),
					0.12f * FlarialGUI::frameFactor);

				FlarialGUI::lerp(searchCutOutHeights[index], Constraints::RelativeConstraint(0.395f, "height"),
					0.12f * FlarialGUI::frameFactor);
			}
			else {

				FlarialGUI::lerp(searchBarSizes[index], Constraints::RelativeConstraint(0.42, "height"),
					0.12f * FlarialGUI::frameFactor);

				FlarialGUI::lerp(searchCutOutHeights[index], -0.5f, 0.12f * FlarialGUI::frameFactor);
			}

			col = clickgui->getColor("primary1", "ClickGUI");

			D2D1_COLOR_F searchbg = clickgui->getColor("secondary4", "ClickGUI");


			if (FlarialGUI::TextBoxes[index].isActive) FlarialGUI::RoundedRect(x - textWidth, y, searchbg, textWidth, percHeight, round.x, round.x, ImDrawFlags_RoundCornersBottom);
			else  FlarialGUI::RoundedRect(x - textWidth, y, searchbg, textWidth, percHeight, round.x, round.x, ImDrawFlags_RoundCornersAll);


			ImVec2 pos{ x - textWidth, y + searchCutOutHeights[index] };
			ImVec2 size{ textWidth, percHeight - searchCutOutHeights[index] };

			FlarialGUI::RoundedRect(pos.x, pos.y, col, size.x, size.y, round.x, round.y);


			/*IDWriteTextLayout* textLayout = FlarialGUI::GetTextLayout(
				FlarialGUI::to_wide(text).c_str(),
				DWRITE_TEXT_ALIGNMENT_LEADING,
				DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
				Constraints::SpacingConstraint(0.60f, textWidth),
				DWRITE_FONT_WEIGHT_REGULAR, textWidth, percHeight);
			*/

			D2D1_COLOR_F cursorCol = D2D1::ColorF(D2D1::ColorF::White);

			cursorCol.a = FlarialGUI::TextBoxes[index].cursorOpac;

			FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorX,
				(x - textWidth) + Constraints::RelativeConstraint(0.38, "height") +
				FlarialGUI::TextSizes[name], 0.420f * FlarialGUI::frameFactor);

			FlarialGUI::RoundedRect(FlarialGUI::TextBoxes[index].cursorX,
				y + Constraints::RelativeConstraint(0.2f) / 2.0f, cursorCol,
				Constraints::RelativeConstraint(0.025f), percHeight / 1.9f, 0, 0);

			if (searchBarSizes[index] > Constraints::RelativeConstraint(0.45, "height")) {
				name = FlarialGUI::FlarialTextWithFont((x - textWidth) + Constraints::RelativeConstraint(0.38, "height"),
					y,
					FlarialGUI::to_wide(text).c_str(),
					textWidth, percHeight,
					DWRITE_TEXT_ALIGNMENT_LEADING,
					Constraints::SpacingConstraint(0.60f, textWidth),
					DWRITE_FONT_WEIGHT_NORMAL, false, true);

			}

			FlarialGUI::image(IDR_SEARCH_PNG,
				D2D1::RectF((x - textWidth) + Constraints::RelativeConstraint(0.245, "height") / 2.0f,
					y + Constraints::RelativeConstraint(0.245, "height") / 2.0f,
					((x - textWidth) +
						Constraints::RelativeConstraint(0.18, "height") / 2.0f) +
					Constraints::RelativeConstraint(0.22, "height"),
					(y + Constraints::RelativeConstraint(0.18, "height") / 2.0f) +
					Constraints::RelativeConstraint(0.22, "height")));
			return "";
		}
	}

	return "";
}