#include "ClickGui.h"

void ClickGui::onKey(uintptr_t key, bool held, bool& cancel) {
	if (enabled && key != VK_F11) {
		cancel = true;
	}

	if (key == keybind && held) {
		this->enabled = true;
	}

	if (key == VK_ESCAPE && held) {
		this->enabled = false;
	}
}

void ClickGui::onMouse(MouseAction action, bool& cancel) {
	if (enabled) {
		cancel = true;
	}
}

void ClickGui::onRender() {
	if (!enabled) {
		return;
	}

	ImDrawList* draw = ImGui::GetForegroundDrawList();

	ClientInstance* instance = MC::getClientInstance();
	std::string currScreenName = instance->getTopScreenName();

	/* if (currScreenName.rfind("hud_screen") == std::string::npos) {

		this->enabled = false;
		return;

	}; */

	// instance->releaseMouse();

	float extraXWinSpace = 200.f;

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImVec2 windowSize = ImVec2(displaySize.x / 2, displaySize.y / 2);
	ImGui::SetNextWindowPos(ImVec2(((displaySize.x - extraXWinSpace) - windowSize.x) / 2, (displaySize.y - windowSize.y) / 2));
	ImGui::SetNextWindowSize(ImVec2(windowSize.x + extraXWinSpace, windowSize.y));
	ImGui::SetNextWindowContentSize(ImVec2(windowSize.x, windowSize.y + 100.f));

	// ImVec4 window_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	// window_color.w = 0.84f;

	// ImGui::PushStyleColor(ImGuiCol_WindowBg, window_color);

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));

	ImGuiWindowFlags windowFlags = {
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar
	};

	if (ImGui::Begin(std::string(std::string(ICON_FA_ATOM) + " " + "Flarial").c_str(), NULL, windowFlags)) {
		auto font = *ImGui::GetFont();
		font.Scale = 1.f;

		ImGui::PushFont(&font);
		// ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.f, 6.f));
		// ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.23, 0.78f, 0.51, 1.0f));

		/* Navbar Contents */

		if (ImGui::Button("Modules")) {

			currWindow = 0;

		}; ImGui::SameLine();

		if (ImGui::Button("Friends")) {

			currWindow = 1;

		}; ImGui::SameLine();

		if (ImGui::Button("Settings")) {

			currWindow = 2;

		}; ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (ImGui::CalcTextSize("X").x * 6.f) - ImGui::GetStyle().WindowPadding.x);

		if (ImGui::Button(" X "))
			this->enabled = false;

		/* End Of Navbar Contents */

		/* Windows */

		/* if (ImGui::BeginChild("Module Configs", ImVec2(140.f, 0.f), true)) {

			auto font = *ImGui::GetFont();
			font.Scale = .6f;

			ImGui::PushFont(&font);

			// if (moduleConfig)
				// moduleConfig->callEvent<ClickGuiModConfigEvent>(ClickGuiModConfigEvent{});

			ImGui::PopFont();

			ImGui::EndChild();

		}; ImGui::SameLine(); */

		ImGui::Separator();

		if (currWindow == 0) {

			auto font = *ImGui::GetFont();
			font.Scale = 1.f;

			ImGui::PushFont(&font);

			if (moduleConfig != nullptr) {
				for (BaseSetting* setting : moduleConfig->settings) {
					if (setting->visible) {
						switch (setting->type)
						{
						case Bool:
						{
							BooleanSetting* boolSetting = (BooleanSetting*)setting;
							ImGui::Checkbox(boolSetting->name.c_str(), boolSetting->status);
							break;
						}
						case Float:
						{
							FloatSetting* floatSetting = (FloatSetting*)setting;
							ImGui::SliderFloat(floatSetting->name.c_str(), floatSetting->current, floatSetting->min, floatSetting->max);
							break;
						}
						case Color:
						{
							ColorSetting* colorSetting = (ColorSetting*)setting;
							ImGui::ColorEdit4(colorSetting->name.c_str(), colorSetting->color);
							break;
						}
						default:
							break;
						}

						if (ImGui::IsItemHovered() && !setting->tooltip.empty())
							ImGui::SetTooltip(setting->tooltip.c_str());
					}
				}
			}
			else {
				auto I = 0;
				for (Module* mod : Manager::modules) {

					I++;

					if (ImGui::BeginChild(std::string("Card-" + mod->name).c_str(), ImVec2((ImGui::GetWindowWidth() - 60.f) / 3.f, 150.f), true)) {

						ImGui::Text(std::string(mod->icon + " " + mod->name).c_str());

						ImGui::Spacing();
						ImGui::Spacing();

						if (ImGui::Button(ICON_FA_COG))
							moduleConfig = mod;

						ImGui::SameLine();

						bool isEnabled = mod->enabled;

						if (!isEnabled)
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.89, 0.31f, 0.23, 1.0f));

						if (ImGui::Button(mod->enabled ? "Enabled" : "Disabled"))
							mod->enabled = !mod->enabled;

						if (!isEnabled)
							ImGui::PopStyleColor();

						ImGui::EndChild();

					};

					if (I == 3)
						I = 0;
					else
						ImGui::SameLine();

				};
			}

			ImGui::PopFont();

		};

		/* End Of Windows */

		// ImGui::PopStyleColor();
		// ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopFont();
		ImGui::End();

	};

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	// ImGui::PopStyleColor();
}