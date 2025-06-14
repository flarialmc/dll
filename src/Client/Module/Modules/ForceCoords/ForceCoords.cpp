#include "ForceCoords.hpp"

void ForceCoords::onEnable() {
    Listen(this, RenderEvent, &ForceCoords::onRender)
    Module::onEnable();
}

void ForceCoords::onDisable() {
    Deafen(this, RenderEvent, &ForceCoords::onRender)
    unpatch();
    Module::onDisable();
}

void ForceCoords::patch() {
    if (patched) return;
    patched = true;
    DWORD oldProtect;
    VirtualProtect((LPVOID)option, patched_option.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((LPVOID)option, patched_option.data(), patched_option.size());
    VirtualProtect((LPVOID)option, patched_option.size(), oldProtect, &oldProtect);
}

void ForceCoords::unpatch() {
    if (!patched) return;
    patched = false;
    DWORD oldProtect;
    VirtualProtect((LPVOID)option, original_option.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((LPVOID)option, original_option.data(), original_option.size());
    VirtualProtect((LPVOID)option, original_option.size(), oldProtect, &oldProtect);
}

void ForceCoords::defaultConfig() {
    Module::defaultConfig("all");
    setDef("MojangStyle", false);
    setDef("textscale", 1.00f);
    if (ModuleManager::initialized) Client::SaveSettings();
}

void ForceCoords::settingsRender(float settingsOffset) {
    return;
    ///* Border Start */
    //
    //float toggleX = Constraints::PercentageConstraint(0.019, "left");
    //float toggleY = Constraints::PercentageConstraint(0.10, "top");
    //
    //const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
    //const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);
    //
    //FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
    //FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
    //	Constraints::RelativeConstraint(1.0, "width"),
    //	Constraints::RelativeConstraint(1.0f, "height"));
    //
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 3.0f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //
    //float percent = FlarialGUI::Slider(3, toggleX + FlarialGUI::SettingsTextWidth("UI Scale "),
    //	toggleY, getOps<float>("uiscale"), 2.0f);
    //
    //getOps<float>("uiscale") = percent;
    //
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //if (FlarialGUI::Toggle(0, toggleX, toggleY, this->settings.getSettingByName<bool>(
    //	"border")->value))
    //	getOps<bool>("border") = !this->settings.getSettingByName<bool>(
    //		"border")->value;
    //FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, L"Border",
    //	textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //
    //
    //percent = FlarialGUI::Slider(1, toggleX + FlarialGUI::SettingsTextWidth("Border ") +
    //	Constraints::SpacingConstraint(0.60, textWidth),
    //	toggleY, getOps<float>("borderWidth"), 4);
    //
    //getOps<float>("borderWidth") = percent;
    //
    ///* Border End */
    //
    ///* Rounding Start */
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //
    //
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rounding", textWidth * 3.0f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //
    //percent = FlarialGUI::Slider(2, toggleX + FlarialGUI::SettingsTextWidth("Rounding "),
    //	toggleY, getOps<float>("rounding"));
    //
    //getOps<float>("rounding") = percent;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //
    //FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
    //	L"Translucency", textWidth * 6.9f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //
    //if (FlarialGUI::Toggle(4, toggleX, toggleY, this->settings.getSettingByName<bool>(
    //	"BlurEffect")->value))
    //	getOps<bool>("BlurEffect") = !this->settings.getSettingByName<bool>(
    //		"BlurEffect")->value;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::TextBoxVisual(7, getOps<std::string>("text"), 16, toggleX, toggleY);
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text Scale", textWidth * 3.0f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //percent = FlarialGUI::Slider(8, toggleX + FlarialGUI::SettingsTextWidth("Text Scale "),
    //	toggleY, getOps<float>("textscale"), 2.00);
    //getOps<float>("textscale") = percent;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
    //	L"Mojang Style", textWidth * 6.9f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //if (FlarialGUI::Toggle(9, toggleX, toggleY, this->settings.getSettingByName<bool>(
    //	"MojangStyle")->value))
    //	getOps<bool>("MojangStyle") = !this->settings.getSettingByName<bool>(
    //		"MojangStyle")->value;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //
    //
    //std::string txtAlignment = FlarialGUI::Dropdown(1,
    //	toggleX, toggleY,
    //	std::vector<std::string>{"Left", "Center", "Right"},
    //	this->settings.getSettingByName<std::string>(
    //		"textalignment")->value,
    //	"Text Alignment"
    //);
    //
    //getOps<std::string>("textalignment") = txtAlignment;
    //
    //FlarialGUI::SetIsInAdditionalYMode();
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
    //	L"Reverse Padding X", textWidth * 6.9f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //if (FlarialGUI::Toggle(15, toggleX, toggleY, this->settings.getSettingByName<bool>(
    //	"reversepaddingx")->value))
    //	getOps<bool>("reversepaddingx") = !this->settings.getSettingByName<bool>(
    //		"reversepaddingx")->value;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
    //	L"Reverse Padding Y", textWidth * 6.9f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //if (FlarialGUI::Toggle(16, toggleX, toggleY, this->settings.getSettingByName<bool>(
    //	"reversepaddingy")->value))
    //	getOps<bool>("reversepaddingy") = !this->settings.getSettingByName<bool>(
    //		"reversepaddingy")->value;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Padding X", textWidth * 3.0f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //percent = FlarialGUI::Slider(15, toggleX + FlarialGUI::SettingsTextWidth("Padding X "),
    //	toggleY, getOps<float>("padx"), 0.53f, 0.0f,
    //	false);
    //
    //getOps<float>("padx") = percent;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Padding Y", textWidth * 3.0f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //percent = FlarialGUI::Slider(16, toggleX + FlarialGUI::SettingsTextWidth("Padding Y "),
    //	toggleY, getOps<float>("pady"), 0.53f, 0.0f,
    //	false);
    //
    //getOps<float>("pady") = percent;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rect Width", textWidth * 3.0f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //percent = FlarialGUI::Slider(17, toggleX + FlarialGUI::SettingsTextWidth("Rect Width "),
    //	toggleY, getOps<float>("rectwidth"), 3.0f);
    //
    //getOps<float>("rectwidth") = percent;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rect Height", textWidth * 3.0f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //percent = FlarialGUI::Slider(18, toggleX + FlarialGUI::SettingsTextWidth("Rect Height "),
    //	toggleY, getOps<float>("rectheight"), 3.0f);
    //
    //getOps<float>("rectheight") = percent;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
    //	L"Responsive Rectangle (Moves with the Text)",
    //	textWidth * 5.f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::SpacingConstraint(1.05, textWidth),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //if (FlarialGUI::Toggle(17, toggleX, toggleY, this->settings.getSettingByName<bool>(
    //	"responsivewidth")->value))
    //	getOps<bool>("responsivewidth") = !this->settings.getSettingByName<bool>(
    //		"responsivewidth")->value;
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rotation", textWidth * 3.0f, textHeight,
    //	DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::RelativeConstraint(0.12, "height", true),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //percent = FlarialGUI::Slider(19, toggleX + FlarialGUI::SettingsTextWidth("Rotation "),
    //	toggleY, getOps<float>("rotation"), 359.0f, 0.0f,
    //	false);
    //
    //getOps<float>("rotation") = percent;
    //
    //FlarialGUI::UnSetIsInAdditionalYMode();
    //
    ///* Rounding End */
    //
    /* Color Pickers Start*/

    //toggleX = Constraints::PercentageConstraint(0.55, "left");
    //toggleY = Constraints::PercentageConstraint(0.10, "top");
    //
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Background", textWidth * 6.9f,
    //	textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::SpacingConstraint(1.05, textWidth),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //FlarialGUI::ColorPicker(0, toggleX + FlarialGUI::SettingsTextWidth("Background "),
    //	toggleY - Constraints::SpacingConstraint(0.017, textWidth),
    //	getOps<std::string>("bgColor"),
    //	getOps<bool>("bgRGB"));
    //
    //toggleX = Constraints::PercentageConstraint(0.55, "left");
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text", textWidth * 6.9f,
    //	textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::SpacingConstraint(1.05, textWidth),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //FlarialGUI::ColorPicker(1, toggleX + FlarialGUI::SettingsTextWidth("Text "), toggleY * 0.99f,
    //	getOps<std::string>("textColor"),
    //	getOps<bool>("textRGB"));
    //
    //toggleY += Constraints::SpacingConstraint(0.35, textWidth);
    //
    //FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Border", textWidth * 6.9f,
    //	textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
    //	Constraints::SpacingConstraint(1.05, textWidth),
    //	DWRITE_FONT_WEIGHT_NORMAL);
    //FlarialGUI::ColorPicker(2, toggleX + FlarialGUI::SettingsTextWidth("Border "), toggleY * 0.99f,
    //	getOps<std::string>("borderColor"),
    //	getOps<bool>("borderRGB"));
    //
    //FlarialGUI::UnsetScrollView();
    //
    //FlarialGUI::ColorPickerWindow(0, getOps<std::string>("bgColor"),
    //	getOps<float>("bgOpacity"),
    //	getOps<bool>("bgRGB"));
    //FlarialGUI::ColorPickerWindow(1, getOps<std::string>("textColor"),
    //	getOps<float>("textOpacity"),
    //	getOps<bool>("textRGB"));
    //FlarialGUI::ColorPickerWindow(2, getOps<std::string>("borderColor"),
    //	getOps<float>("borderOpacity"),
    //	getOps<bool>("borderRGB"));
    ///* Color Pickers End */
    //
}

void ForceCoords::onRender(RenderEvent &event) {


    if (ClientInstance::getTopScreenName() == "hud_screen") {

        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

            if (SDK::clientInstance->getLocalPlayer() != nullptr) {

                if (getOps<bool>("MojangStyle") && !mojanged) {
                    patch();
                    mojanged = true;
                }
                else if (!getOps<bool>("MojangStyle")) {
                    if (mojanged) {
                        unpatch();
                        mojanged = false;
                    }

                    Vec3<float> Pos = SDK::clientInstance->getLocalPlayer()->getAABBShapeComponent()->aabb.lower;
                    //Vec3<float> PrevPos = SDK::clientInstance->getLocalPlayer()->stateVector->PrevPos;
                    //Vec3<float> vel = SDK::clientInstance->getLocalPlayer()->stateVector->velocity;

                    std::string cords = FlarialGUI::cached_to_string(static_cast<int>(Pos.x)) + ", " +
                                        FlarialGUI::cached_to_string(static_cast<int>(Pos.y)) + ", " +
                                        FlarialGUI::cached_to_string(static_cast<int>(Pos.z));
                    //std::string cords1 = FlarialGUI::cached_to_string(static_cast<int>(PrevPos.x)) + ", " + FlarialGUI::cached_to_string(static_cast<int>(PrevPos.y)) + ", " + FlarialGUI::cached_to_string(static_cast<int>(PrevPos.z));
                    //std::string cords2 = FlarialGUI::cached_to_string(static_cast<int>(vel.x)) + ", " + FlarialGUI::cached_to_string(static_cast<int>(vel.y)) + ", " + FlarialGUI::cached_to_string(static_cast<int>(vel.z));
                    this->normalRender(6, cords);
                }
            }
        }
    }
    else if (mojanged) {
        unpatch();
        mojanged = false;
    }
}
