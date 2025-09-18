#include "Engine.hpp"
#include "../../Client.hpp"
#include "../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include <imgui/imgui_internal.h>
#include "Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

// Essential DirectX headers for window management
#include <d2d1.h>

// Window management, scrolling, clipping, and utility functions

void FlarialGUI::SetScrollView(float x, float y, float width, float height) {
	FlarialGUI::isInScrollView = true;
	D2D1_RECT_F clipRect = D2D1::RectF(x, y, x + width, y + height);
	ScrollViewRect = clipRect;
	PushImClipRect(clipRect);
	D2D::context->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

}

void FlarialGUI::UnsetScrollView() {
	FlarialGUI::isInScrollView = false;
	PopImClipRect();
	D2D::context->PopAxisAlignedClip();
}

void FlarialGUI::SetWindowRect(float x, float y, float width, float height, int currentNum, std::string modname) {
	if (MC::mouseButton == MouseButton::Right) return;
	float fixer = 0;
	isInWindowRect = true;

	D2D1_COLOR_F c = D2D1::ColorF(D2D1::ColorF::White);
	c.a = 0.5f;
	FlarialGUI::RoundedRect(x, y, c, width, height, 0, 0);
	c.a = 1.f;
	FlarialGUI::RoundedHollowRect(x, y, Constraints::RelativeConstraint(0.003f, "height", true), c, width, height, 0, 0);
	FlarialGUI::FlarialTextWithFont(x, y + 0.005f * MC::windowSize.y, FlarialGUI::to_wide(modname).c_str(), 0, 0, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.1f, "height", true), DWRITE_FONT_WEIGHT_NORMAL, false);

	if (currentNum > maxRect) maxRect = currentNum;

	int i = 0;
	bool ye = false;
	for (const auto& [key, rect] : WindowRects) {
		if (rect.isMovingElement && key != currentNum) {
			ye = true;
			break;
		}
	}

	WindowRects[currentNum].width = width;
	WindowRects[currentNum].height = height;

	if (CursorInRect(x, y + (isInScrollView ? scrollpos : 0), width, height) && !WindowRects[currentNum].isHovering) {
		WindowRects[currentNum].isHovering = true;
		WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::SizeAll);
	}
	else if (!CursorInRect(x, y + (isInScrollView ? scrollpos : 0), width, height) && WindowRects[currentNum].isHovering) {
		WindowRects[currentNum].isHovering = false;
		WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
	}

	if (!ye) {
		if ((CursorInRect(x, y, width, height) || WindowRects[currentNum].isMovingElement) && MC::held) {
			if (!WindowRects[currentNum].isMovingElement) {
				WindowRects[currentNum].oriMouseX = (MC::mousePos.x - ((x + width / 2.0f)));
				WindowRects[currentNum].oriMouseY = (MC::mousePos.y - ((y + height / 2.0f)));
				WindowRects[currentNum].isMovingElement = true;
			}
			WindowRects[currentNum].hasBeenMoved = true;

			WindowRects[currentNum].movedX = ((MC::mousePos.x - WindowRects[currentNum].oriMouseX) - width / 2.0f) + fixer;
			WindowRects[currentNum].movedY = ((MC::mousePos.y - WindowRects[currentNum].oriMouseY) - height / 2.0f);
		}

		if (MC::mouseButton == MouseButton::None && !MC::held || MC::mouseButton == MouseButton::Left && !MC::held) {
			WindowRects[currentNum].isMovingElement = false;
			WindowRects[currentNum].oriMouseX = -1;
			WindowRects[currentNum].oriMouseY = -1;
		}
	}

	WindowRects[currentNum].fixer = fixer;
	if (WindowRects[currentNum].movedX - fixer < 0) WindowRects[currentNum].movedX = 0.001f + fixer;
	if (WindowRects[currentNum].movedY < 0) WindowRects[currentNum].movedY = 0;

	if (WindowRects[currentNum].movedX + width - fixer > MC::windowSize.x) WindowRects[currentNum].movedX = MC::windowSize.x - width + fixer;
	if (WindowRects[currentNum].movedY + height > MC::windowSize.y) WindowRects[currentNum].movedY = MC::windowSize.y - height;

	WindowRects[currentNum].percentageX = WindowRects[currentNum].movedX / MC::windowSize.x;
	WindowRects[currentNum].percentageY = WindowRects[currentNum].movedY / MC::windowSize.y;

	if (WindowRects[currentNum].isMovingElement && Client::settings.getSettingByName<bool>("snappinglines")->value && !MC::holdingCTRL) {
		const float alignmentThreshold = 10.0f;
		const ImColor pink(1.0f, 0.0f, 1.0f, 1.0f);

		for (int j = 0; j <= maxRect; j++) {
			if (j == currentNum) continue;

			auto it = WindowRects.find(j);
			if (it != WindowRects.end()) {
				auto& otherRect = it->second;

				if (fabs(WindowRects[currentNum].movedX - otherRect.movedX - fixer) < alignmentThreshold) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX, 0), ImVec2(otherRect.movedX, MC::windowSize.y), pink, 2.0f);
					WindowRects[currentNum].movedX = otherRect.movedX + fixer;
				}

				if (fabs(WindowRects[currentNum].movedX + width - (otherRect.movedX + otherRect.width) - fixer) < alignmentThreshold) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX + otherRect.width, 0), ImVec2(otherRect.movedX + otherRect.width, MC::windowSize.y), pink, 2.0f);
					WindowRects[currentNum].movedX = otherRect.movedX + otherRect.width - width + fixer;
				}

				if (fabs(WindowRects[currentNum].movedY - otherRect.movedY) < alignmentThreshold) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY), ImVec2(MC::windowSize.x, otherRect.movedY), pink, 2.0f);
					WindowRects[currentNum].movedY = otherRect.movedY;
				}

				if (fabs(WindowRects[currentNum].movedY + height - (otherRect.movedY + otherRect.height)) < alignmentThreshold) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY + otherRect.height), ImVec2(MC::windowSize.x, otherRect.movedY + otherRect.height), pink, 2.0f);
					WindowRects[currentNum].movedY = otherRect.movedY + otherRect.height - height;
				}

				if (fabs((WindowRects[currentNum].movedX + width / 2.0f) - (otherRect.movedX + otherRect.width / 2.0f) - fixer) < alignmentThreshold) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(otherRect.movedX + otherRect.width / 2.0f, 0), ImVec2(otherRect.movedX + otherRect.width / 2.0f, MC::windowSize.y), pink, 2.0f);
					WindowRects[currentNum].movedX = otherRect.movedX + otherRect.width / 2.0f - width / 2.0f + fixer;
				}

				if (fabs((WindowRects[currentNum].movedY + height / 2.0f) - (otherRect.movedY + otherRect.height / 2.0f)) < alignmentThreshold) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, otherRect.movedY + otherRect.height / 2.0f), ImVec2(MC::windowSize.x, otherRect.movedY + otherRect.height / 2.0f), pink, 2.0f);
					WindowRects[currentNum].movedY = otherRect.movedY + otherRect.height / 2.0f - height / 2.0f;
				}
			}
		}

		if (fabs(WindowRects[currentNum].movedX - fixer) < alignmentThreshold) {
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(WindowRects[currentNum].movedX, 0),
				ImVec2(WindowRects[currentNum].movedX, MC::windowSize.y), pink, 2.0f);
			WindowRects[currentNum].movedX = fixer;
		}

		if (fabs(WindowRects[currentNum].movedX + width - MC::windowSize.x - fixer) < alignmentThreshold) {
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(WindowRects[currentNum].movedX + width, 0),
				ImVec2(WindowRects[currentNum].movedX + width, MC::windowSize.y), pink, 2.0f);
			WindowRects[currentNum].movedX = MC::windowSize.x - width + fixer;
		}

		if (fabs(WindowRects[currentNum].movedY) < alignmentThreshold) {
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, WindowRects[currentNum].movedY),
				ImVec2(MC::windowSize.x, WindowRects[currentNum].movedY), pink, 2.0f);
			WindowRects[currentNum].movedY = 0.0f;
		}

		if (fabs(WindowRects[currentNum].movedY + height - MC::windowSize.y) < alignmentThreshold) {
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, WindowRects[currentNum].movedY + height),
				ImVec2(MC::windowSize.x, WindowRects[currentNum].movedY + height), pink, 2.0f);
			WindowRects[currentNum].movedY = MC::windowSize.y - height;
		}

		if (fabs((WindowRects[currentNum].movedX + width / 2.0f - MC::windowSize.x / 2.0f) - fixer) < alignmentThreshold) {
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(MC::windowSize.x / 2.0f, 0),
				ImVec2(MC::windowSize.x / 2.0f, MC::windowSize.y), pink, 2.0f);
			WindowRects[currentNum].movedX = MC::windowSize.x / 2.0f - width / 2.0f + fixer;
		}

		if (fabs(WindowRects[currentNum].movedY + height / 2.0f - MC::windowSize.y / 2.0f) < alignmentThreshold) {
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(0, MC::windowSize.y / 2.0f),
				ImVec2(MC::windowSize.x, MC::windowSize.y / 2.0f), pink, 2.0f);
			WindowRects[currentNum].movedY = MC::windowSize.y / 2.0f - height / 2.0f;
		}
	}
}

void FlarialGUI::UnsetWindowRect() {
	isInWindowRect = false;
}

Vec2<float> FlarialGUI::CalculateMovedXY(float x, float y, int num, float rectWidth, float rectHeight) {
	if (isInWindowRect && WindowRects[num].hasBeenMoved) {

		WindowRects[num].percentageX = WindowRects[num].movedX / MC::windowSize.x;
		WindowRects[num].percentageY = WindowRects[num].movedY / MC::windowSize.y;

		x = (WindowRects[num].percentageX * MC::windowSize.x);
		y = (WindowRects[num].percentageY * MC::windowSize.y);

	}

	if (x - WindowRects[num].fixer < 0) x = 0.001f - WindowRects[num].fixer;
	if (y < 0) y = 0;

	if (x + rectWidth - WindowRects[num].fixer > MC::windowSize.x) x = MC::windowSize.x + WindowRects[num].fixer;
	if (y + rectHeight > MC::windowSize.y) y = MC::windowSize.y;

	if (x == 0) x = 0.0001f;
	if (y == 0) y = 0.0001f;

	return Vec2<float>{x, y};
}

Vec2<float> FlarialGUI::GetCenterXY(float rectWidth, float rectHeight) {
	Vec2<float> xy;
	xy.x = (D2D::context->GetSize().width - rectWidth) / 2.0f;
	xy.y = (D2D::context->GetSize().height - rectHeight) / 2.0f;
	return xy;
}

void FlarialGUI::ResetShit() {
	for (auto& i : WindowRects) {
		i.second = WindowRect();
	}

	for (auto& i : SliderRects) {
		i.second = SliderRect();
	}

	for (auto& i : TextBoxes) {
		i.second = TextBoxStruct();
	}

	for (auto& i : ColorPickers) {
		i.second = ::ColorPicker();
	}

	for (auto& i : DropDownMenus) {
		i.second = DropdownStruct();
	}

	activeColorPickerWindows = 0;
}

static int rotation_start_index;
void FlarialGUI::ImRotateStart()
{
	rotation_start_index = ImGui::GetBackgroundDrawList()->VtxBuffer.Size;
}

ImVec2 FlarialGUI::ImRotationCenter()
{
	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

	const auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

	return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
}

void FlarialGUI::ImRotateEnd(float angle, ImVec2 center)
{
	float rad = angle * IM_PI / 180.0f;
	float s = sin(rad), c = cos(rad);
	center = ImRotate(center, s, c) - center;

	auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

std::vector<D2D_RECT_F> PreviousClippingRects = {};

void FlarialGUI::PushImClipRect(ImVec2 pos, ImVec2 size, bool overridePreviousClipping) {

	ImVec2 max(pos.x + size.x, pos.y + size.y);

    if (!overridePreviousClipping and !PreviousClippingRects.empty())
    {
        ImVec2 ClippedPos(PreviousClippingRects.back().left, PreviousClippingRects.back().top);
        ImVec2 ClippedSize(PreviousClippingRects.back().right, PreviousClippingRects.back().bottom);

        if (ClippedPos.x >= pos.x) pos.x = ClippedPos.x + 1;
        if (ClippedPos.y >= pos.y) pos.y = ClippedPos.y + 1;
        if (ClippedSize.x <= max.x) max.x = ClippedSize.x - 1;
        if (ClippedSize.y <= max.y) max.y = ClippedSize.y - 1;
    }

	PreviousClippingRects.push_back(D2D_RECT_F(pos.x, pos.y, max.x, max.y));

	ImGui::GetBackgroundDrawList()->PushClipRect(pos, max);
}

void FlarialGUI::PushImClipRect(D2D_RECT_F rect, bool overridePreviousClipping) {
    ImVec2 pos(rect.left, rect.top);
    ImVec2 size(rect.right - rect.left, rect.bottom - rect.top);

	ImVec2 max(pos.x + size.x, pos.y + size.y);

    if (!overridePreviousClipping and !PreviousClippingRects.empty())
    {
        ImVec2 ClippedPos(PreviousClippingRects.back().left, PreviousClippingRects.back().top);
        ImVec2 ClippedSize(PreviousClippingRects.back().right, PreviousClippingRects.back().bottom);

        if (ClippedPos.x >= pos.x) pos.x = ClippedPos.x + 1;
        if (ClippedPos.y >= pos.y) pos.y = ClippedPos.y + 1;
        if (ClippedSize.x <= max.x) max.x = ClippedSize.x - 1;
        if (ClippedSize.y <= max.y) max.y = ClippedSize.y - 1;
    }

    PreviousClippingRects.push_back(D2D_RECT_F(pos.x, pos.y, max.x, max.y));

	ImGui::GetBackgroundDrawList()->PushClipRect(pos, max);
}

void FlarialGUI::PopImClipRect() {
    PreviousClippingRects.pop_back();
    ImGui::GetBackgroundDrawList()->PopClipRect();
}

void FlarialGUI::Notify(const std::string& text) {
	if (SwapchainHook::init) {
		Notification e;
		e.text = text;
		e.finished = false;
		e.currentPosY = MC::windowSize.y;
		e.arrived = false;
		e.finished = false;
		notifications.push_back(e);
	}
}

void FlarialGUI::NotifyHeartbeat() {
	float funnyTroll = 1.f;
	FlarialGUI::lerp(funnyTroll, 40.f, 0.12f * FlarialGUI::frameFactor);

	Vec2<float> round = Constraints::RoundingConstraint(20, 20);
	float height = Constraints::RelativeConstraint(0.035, "height", true);
	float posxModif = 0;
	float fontSize = Constraints::RelativeConstraint(0.128, "height", true);
	float textposyModif = Constraints::RelativeConstraint(0.0045f, "height", true);
	D2D_COLOR_F col = ClickGUI::getColor("primary1");

	int i = 0;
	for (Notification& n : notifications) {

		float posyModif = -((height + Constraints::RelativeConstraint(0.01f, "height", true)) * i);

		if (n.firstTime) {
			float TrollSize = Constraints::RelativeConstraint(0.128, "height", true);
			std::string sizeName = FlarialGUI::FlarialTextWithFont(n.currentPos, n.currentPosY, FlarialGUI::to_wide(n.text).c_str(), 10, 25, DWRITE_TEXT_ALIGNMENT_CENTER, TrollSize, DWRITE_FONT_WEIGHT_NORMAL, D2D1::ColorF(0, 0, 0, 0));
			if (FlarialGUI::TextSizes[sizeName] != 0) {
				n.width = FlarialGUI::TextSizes[sizeName] + Constraints::RelativeConstraint(0.0345f, "height", true);
				n.currentPos = Constraints::CenterConstraint(n.width, 0).x;
				n.firstTime = false;
			}
		}

		if (!n.finished) {
			if (!n.arrived) {

				FlarialGUI::RoundedRect(n.currentPos, n.currentPosY + posyModif, col, n.width, height, round.x, round.x);
				FlarialGUI::FlarialTextWithFont(n.currentPos + posxModif, n.currentPosY + posyModif, FlarialGUI::to_wide(n.text).c_str(), n.width, height, DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL);
				FlarialGUI::lerp(n.currentPosY, Constraints::PercentageConstraint(0.1f, "bottom", true), FlarialGUI::frameFactor * 0.067f);

				if (n.currentPosY <= Constraints::PercentageConstraint(0.08f, "bottom", true)) {
					n.arrived = true;
					n.time = std::chrono::steady_clock::now();
				}

				i++;

			}
			else {

				std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
				auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(current - n.time);

				FlarialGUI::RoundedRect(n.currentPos, n.currentPosY + posyModif, col, n.width, height, round.x, round.x);
				FlarialGUI::FlarialTextWithFont(n.currentPos + posxModif, n.currentPosY + posyModif, FlarialGUI::to_wide(n.text).c_str(), n.width, height, DWRITE_TEXT_ALIGNMENT_CENTER, fontSize, DWRITE_FONT_WEIGHT_NORMAL);

				if (timeDifference.count() > 5000) {
					FlarialGUI::lerp(n.currentPosY, MC::windowSize.y + 500, FlarialGUI::frameFactor * 0.052f);
					if (n.currentPosY >= MC::windowSize.y) { n.finished = true; }
				}

				i++;
			}
		}
		else {
			FlarialGUI::notifications.erase(std::next(FlarialGUI::notifications.begin(), i));
		}
	}
}

void FlarialGUI::CopyBitmap(ID2D1Bitmap1* from, ID2D1Bitmap** to) {
	if (from == nullptr) {
		Logger::custom(fg(fmt::color::dark_golden_rod), "CopyBitmap", "From is nullptr");
		return;
	}

	if (*to == nullptr) {
		D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
		HRESULT hr = D2D::context->CreateBitmap(from->GetPixelSize(), props, to);
		if (FAILED(hr)) {
			Logger::custom(fg(fmt::color::dark_golden_rod), "CopyBitmap", "From is nullptr");
			return;
		}
	}
	else if (from->GetPixelSize() != (*to)->GetPixelSize()) {
		(*to)->Release();
		D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
		HRESULT hr = D2D::context->CreateBitmap(from->GetPixelSize(), props, to);
		if (FAILED(hr)) {
			Logger::custom(fg(fmt::color::dark_golden_rod), "CopyBitmap", "From is nullptr");
			return;
		}
	}

	D2D1_POINT_2U destPoint = { 0, 0 };
	D2D1_SIZE_U size = from->GetPixelSize();
	D2D1_RECT_U rect = { 0, 0, size.width, size.height };
	(*to)->CopyFromBitmap(&destPoint, from, &rect);
}

std::wstring to_wide_creator(const std::string& str) {

	int wchars_num = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	std::wstring wide;
	wide.resize(wchars_num);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wide[0], wchars_num);
	return wide;
}

std::wstring FlarialGUI::to_wide(const std::string& str) {
	return toWideCache.getOrInsert(to_wide_creator, str, str);
}

template<typename T>
void FlarialGUI::lerp(T& a, const T& b, float t) {
	if (!Client::settings.getSettingByName<bool>("disableanims")->value) {
		float interpolatedValue = a + (b - a) * t;
		float roundedValue = std::ceilf(interpolatedValue * 1000.0f) / 1000.0f;
		a = roundedValue;
	}
	else a = b;
}