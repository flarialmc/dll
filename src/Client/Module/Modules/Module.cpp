#include "Module.hpp"

#include "../../Client.hpp"
#include "../../Events/Events.hpp"
#include "ClickGUI/ClickGUI.hpp"
#include "Scripting/ScriptManager.hpp"

#define colors_secondary6 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary6")->value)
#define o_colors_secondary6 clickgui->settings.getSettingByName<float>("o_colors_secondary6")->value
#define colors_secondary6_rgb clickgui->settings.getSettingByName<bool>("colors_secondary6_rgb")->value

std::map<std::string, DWRITE_TEXT_ALIGNMENT> alignments = {
        {"Left", DWRITE_TEXT_ALIGNMENT_LEADING},
        {"Center", DWRITE_TEXT_ALIGNMENT_CENTER},
        {"Right", DWRITE_TEXT_ALIGNMENT_TRAILING}
};

static std::string Lname = "";

void Module::normalRenderCore(int index, std::string& text) {
    float rotation = this->settings.getSettingByName<float>("rotation")->value;
    DWRITE_TEXT_ALIGNMENT alignment = alignments[this->settings.getSettingByName<std::string>(
        "textalignment")->value];
    bool responsivewidth = this->settings.getSettingByName<bool>("responsivewidth")->value;
    float paddingX = this->settings.getSettingByName<float>("padx")->value;
    float paddingY = this->settings.getSettingByName<float>("pady")->value;

    if (this->settings.getSettingByName<bool>("reversepaddingx")->value)
        paddingX = -(this->settings.getSettingByName<float>("padx")->value);
    if (this->settings.getSettingByName<bool>("reversepaddingy")->value)
        paddingY = -(this->settings.getSettingByName<float>("pady")->value);

    float textWidth = Constraints::RelativeConstraint(0.7f * settings.getSettingByName<float>("uiscale")->value);
    float textHeight = Constraints::RelativeConstraint(0.1f * settings.getSettingByName<float>("uiscale")->value);

    float textSize = Constraints::SpacingConstraint(3.2f, textHeight) *
        this->settings.getSettingByName<float>("textscale")->value;

    Vec2<float> settingperc = Vec2<float>(
        this->settings.getSettingByName<float>("percentageX")->value,
        this->settings.getSettingByName<float>("percentageY")->value
    );

    float realspacing = Constraints::SpacingConstraint(0.05f, textWidth);

    std::string rname = FlarialGUI::FlarialTextWithFont(0, 0,
        FlarialGUI::to_wide(text).c_str(),
        1000000,
        textHeight,
        alignment,
        textSize, DWRITE_FONT_WEIGHT_NORMAL,
        D2D1::ColorF(0, 0, 0, 0),
        true
    );

    Lname = rname;

    float rectWidth = (
        !responsivewidth
        ? (Constraints::RelativeConstraint(0.225f * settings.getSettingByName<float>("uiscale")->value) * this->settings.getSettingByName<float>("rectwidth")->value)
        : (FlarialGUI::TextSizes[Lname] + Constraints::SpacingConstraint(2.0, realspacing)) * this->settings.getSettingByName<float>("rectwidth")->value
        );

    Vec2<float> realcenter;

    
    if (settingperc.x != 0) {
        realcenter = Vec2<float>(settingperc.x * (MC::windowSize.x - rectWidth), settingperc.y * (MC::windowSize.y - textHeight));
    }
    else realcenter = Constraints::CenterConstraint(rectWidth, textHeight * this->settings.getSettingByName<float>("rectheight")->value);

    if (ClickGUI::editmenu) {
        auto height = textHeight * this->settings.getSettingByName<float>("rectheight")->value;


        FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, rectWidth, height, index);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, rectWidth, height);

        checkForRightClickAndOpenSettings(realcenter.x, realcenter.y, rectWidth, height);

        realcenter.x = vec2.x;
        realcenter.y = vec2.y;

        realcenter = realcenter;

        Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y, rectWidth, textHeight);
        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);
    }

    Vec2<float> rounde = Constraints::RoundingConstraint(this->settings.getSettingByName<float>("rounding")->value *
        settings.getSettingByName<float>("uiscale")->value,
        this->settings.getSettingByName<float>("rounding")->value *
        settings.getSettingByName<float>("uiscale")->value);

    D2D1_COLOR_F bgColor = settings.getSettingByName<bool>("bgRGB")->value ? FlarialGUI::rgbColor
        : FlarialGUI::HexToColorF(
            settings.getSettingByName<std::string>("bgColor")->value);
    D2D1_COLOR_F textColor = settings.getSettingByName<bool>("textRGB")->value ? FlarialGUI::rgbColor
        : FlarialGUI::HexToColorF(
            settings.getSettingByName<std::string>("textColor")->value);
    D2D1_COLOR_F borderColor = settings.getSettingByName<bool>("borderRGB")->value ? FlarialGUI::rgbColor
        : FlarialGUI::HexToColorF(
            settings.getSettingByName<std::string>("borderColor")->value);

    bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;
    textColor.a = settings.getSettingByName<float>("textOpacity")->value;
    borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;

    ImVec2 rotationCenter = ImVec2(
        realcenter.x + rectWidth / 2.0f,
        realcenter.y + textHeight * this->settings.getSettingByName<float>("rectheight")->value / 2.0f);

    if (rotation > 0.0f) FlarialGUI::ImRotateStart();

    if (settings.getSettingByName<bool>("BlurEffect")->value) {
        FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y,
            realcenter.x + rectWidth,
            realcenter.y + (textHeight)*
            this->settings.getSettingByName<float>(
                "rectheight")->value), rounde.x,
            rounde.x));
    }

    if (this->settings.getSettingByName<bool>("textShadow")->value) {
        textColor = settings.getSettingByName<bool>("textShadowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textShadowCol")->value);
        textColor.a = settings.getSettingByName<float>("textShadowOpacity")->value;
        FlarialGUI::FlarialTextWithFont(
            realcenter.x + Constraints::SpacingConstraint(paddingX, textWidth) + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value),
            realcenter.y + Constraints::SpacingConstraint(paddingY, textWidth) + Constraints::RelativeConstraint(settings.getSettingByName<float>("textShadowOffset")->value),
            FlarialGUI::to_wide(text).c_str(),
            rectWidth,
            textHeight,
            alignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            textColor,
            true
        );
    }

    if (this->settings.getSettingByName<bool>("rectShadow")->value) {
        bgColor = settings.getSettingByName<bool>("rectShadowRGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("rectShadowCol")->value);
        bgColor.a = settings.getSettingByName<float>("rectShadowOpacity")->value;


        FlarialGUI::RoundedRect(
            realcenter.x + Constraints::RelativeConstraint(settings.getSettingByName<float>("rectShadowOffset")->value),
            realcenter.y + Constraints::RelativeConstraint(settings.getSettingByName<float>("rectShadowOffset")->value),
            bgColor,
            rectWidth,
            textHeight * this->settings.getSettingByName<float>("rectheight")->value,
            rounde.x,
            rounde.x
        );
    }

    bgColor = settings.getSettingByName<bool>("bgRGB")->value ? FlarialGUI::rgbColor
        : FlarialGUI::HexToColorF(
            settings.getSettingByName<std::string>("bgColor")->value);
    textColor = settings.getSettingByName<bool>("textRGB")->value ? FlarialGUI::rgbColor
        : FlarialGUI::HexToColorF(
            settings.getSettingByName<std::string>("textColor")->value);
    borderColor = settings.getSettingByName<bool>("borderRGB")->value ? FlarialGUI::rgbColor
        : FlarialGUI::HexToColorF(
            settings.getSettingByName<std::string>("borderColor")->value);

    bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;
    textColor.a = settings.getSettingByName<float>("textOpacity")->value;
    borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;

    FlarialGUI::RoundedRect(
        realcenter.x,
        realcenter.y,
        bgColor,
        rectWidth,
        textHeight * this->settings.getSettingByName<float>("rectheight")->value,
        rounde.x,
        rounde.x
    );

    FlarialGUI::FlarialTextWithFont(
        realcenter.x + Constraints::SpacingConstraint(paddingX, textWidth),
        realcenter.y + Constraints::SpacingConstraint(paddingY, textWidth),
        FlarialGUI::to_wide(text).c_str(),
        rectWidth,
        textHeight,
        alignment,
        textSize, DWRITE_FONT_WEIGHT_NORMAL,
        textColor,
        true
    );


    if (this->settings.getSettingByName<bool>("border")->value) {
        FlarialGUI::RoundedHollowRect(
            realcenter.x,
            realcenter.y,
            Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value *
                settings.getSettingByName<float>("uiscale")->value) / 100.0f,
                "height", true),
            borderColor,
            rectWidth,
            textHeight * this->settings.getSettingByName<float>("rectheight")->value,
            rounde.x,
            rounde.x
        );
    }

    if (rotation > 0.0f) {
        FlarialGUI::ImRotateEnd(rotation, rotationCenter);
    }

    if (ClickGUI::editmenu)
        FlarialGUI::UnsetWindowRect();
}

void Module::normalRender(int index, std::string &value) {
    if(!isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;

    std::string text{};
    if(this->settings.getSettingByName<std::string>("text")!=nullptr)
        text = this->settings.getSettingByName<std::string>("text")->value;

    std::string uppercaseSentence;
    std::string search = "{VALUE}";

    for (char c: text) {
        uppercaseSentence += (char)std::toupper(c);
    }

    size_t pos = uppercaseSentence.find(search);
    if (pos != std::string::npos) {
        text.replace(pos, search.length(), value);
    }

    normalRenderCore(index, text);
}

void Module::resetPadding() {
    padding = 0;
    toggleIndex = 0;
    dropdownIndex = 0;
    sliderIndex = 0;
    buttonIndex = 3;

    int i = 100;
    for (int i = 100; i < colorPickerIndex; ++i) {
        FlarialGUI::ColorPickerWindow(i, *color_pickers[i].value, *color_pickers[i].opacity, *color_pickers[i].rgb);
    }

    colorPickerIndex = 100;
    keybindIndex = 0;
    textboxIndex = 300;
    FlarialGUI::UnSetIsInAdditionalYMode();
}

void Module::extraPadding() {
    padding += Constraints::RelativeConstraint(0.04f, "height", true);
}

void Module::addHeader(std::string text) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;


    D2D1_COLOR_F col = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
    col.a = ClickGUI::settingsOpacity;
    D2D1_COLOR_F textCol = D2D1::ColorF(D2D1::ColorF::White);
    textCol.a = ClickGUI::settingsOpacity;;

    std::string name = FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), 500, 0, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.215f, "height", true), DWRITE_FONT_WEIGHT_BOLD, textCol, false);

    if(FlarialGUI::shouldAdditionalY)
        for (int i = 0; i < FlarialGUI::highestAddIndexes + 1; i++) {
            if (i <= FlarialGUI::additionalIndex && FlarialGUI::additionalY[i] > 0.0f) {
                y += FlarialGUI::additionalY[i];
            }
        }
    FlarialGUI::RoundedRect(x, y + Constraints::RelativeConstraint(0.023f, "width"), col, FlarialGUI::TextSizes[name] + Constraints::RelativeConstraint(0.01f, "width"), 3.0f, 0, 0);

    padding += Constraints::RelativeConstraint(0.055f, "height", true);
}

void Module::addButton(const std::string& text, const std::string& subtext, const std::string& buttonText, std::function<void()> action) {
    float elementX = Constraints::PercentageConstraint(0.15f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;
    const float width = Constraints::RelativeConstraint(0.09f, "height", true);
    const float height = Constraints::RelativeConstraint(0.035, "height", true);
    Vec2<float> round = Constraints::RoundingConstraint(13, 13);
    D2D1_COLOR_F col = FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary1")->value);
    col.a = ClickGUI::settingsOpacity;
    D2D1_COLOR_F f = D2D1::ColorF(D2D1::ColorF::White);
    f.a = ClickGUI::settingsOpacity;

    if(FlarialGUI::RoundedButton(buttonIndex, elementX, y, col, f, FlarialGUI::to_wide(buttonText).c_str(), width, height, round.x, round.y)) {
        action();
    }

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.06f, "height", true);
    buttonIndex++;
}

void Module::addColorPicker(std::string text, std::string subtext, std::string& value, float& opacity, bool& rgb) {
    float elementX = Constraints::PercentageConstraint(0.195f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    FlarialGUI::ColorPicker(colorPickerIndex, elementX, y, value, rgb);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);

    DrDisrespect respect = { &value, &opacity, &rgb };
    color_pickers[colorPickerIndex] = respect;
    colorPickerIndex++;
}

void Module::addTextBox(std::string text, std::string subtext, std::string& value, int limit) {
    float x = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    FlarialGUI::TextBoxVisual(textboxIndex, value, limit, x , y);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    textboxIndex++;
}

void Module::addDropdown(std::string text, std::string subtext, const std::vector<std::string>& options, std::string& value) {
    float x = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    Module::addElementText(text, subtext);


    FlarialGUI::Dropdown(dropdownIndex, x, y, options, value, "");

    if(FlarialGUI::additionalY[dropdownIndex] > 0.f) FlarialGUI::SetIsInAdditionalYMode();

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    dropdownIndex++;
}

void Module::addConditionalSlider(bool condition, std::string text, std::string subtext, float& value, float maxVal, float minVal, bool zerosafe) {
    
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalSliderAnims[sliderIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalSliderAnims[sliderIndex];
        FlarialGUI::lerp(conditionalSliderAnims[sliderIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addSlider(text, subtext, value, maxVal, minVal, zerosafe);
    }
    else {
        FlarialGUI::lerp(conditionalSliderAnims[sliderIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalSliderAnims[sliderIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalSliderAnims[sliderIndex];
            Module::addSlider(text, subtext, value, maxVal, minVal, zerosafe);
        }
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addElementText(std::string text, std::string subtext) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    float subtextY;
    float fontSize = Constraints::RelativeConstraint(0.140f, "height", true);
    float fontSize2 = Constraints::RelativeConstraint(0.132f, "height", true);

    if (!subtext.empty()) {
        subtextY = y;
        y -= Constraints::RelativeConstraint(0.009f, "height", true);
        subtextY += Constraints::RelativeConstraint(0.009f, "height", true);
    }
    else {
        y += Constraints::RelativeConstraint(0.0015f, "height", true);
    }

    D2D1_COLOR_F textCol = D2D1::ColorF(D2D1::ColorF::White);
    D2D1_COLOR_F subtextCol = FlarialGUI::HexToColorF("473b3d");
    textCol.a = o_colors_text;
    subtextCol.a = o_colors_text;

    if(ClickGUI::settingsOpacity != 1) {
        textCol.a = ClickGUI::settingsOpacity;
        subtextCol.a = ClickGUI::settingsOpacity;
    }

    FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), 200, 0, DWRITE_TEXT_ALIGNMENT_LEADING, fontSize, DWRITE_FONT_WEIGHT_MEDIUM, textCol, false);
    if (!subtext.empty()) FlarialGUI::FlarialTextWithFont(x, subtextY, FlarialGUI::to_wide(subtext).c_str(), 200, 0, DWRITE_TEXT_ALIGNMENT_LEADING, fontSize2, DWRITE_FONT_WEIGHT_MEDIUM, subtextCol, false);
}

void Module::addSlider(std::string text, std::string subtext, float& value, float maxVal, float minVal, bool zerosafe) {
    float elementX = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    if (value > maxVal) value = maxVal;
    else if (value < minVal) value = minVal;

    FlarialGUI::Slider(sliderIndex, elementX, y, value, maxVal, minVal, zerosafe);

    Module::addElementText(text, subtext);
    
    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    sliderIndex++;
}

void Module::addToggle(std::string text, std::string subtext, bool& value) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float elementX = Constraints::PercentageConstraint(0.119f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    if (FlarialGUI::Toggle(toggleIndex, elementX, y, value, false)) value = !value;

    Module::addElementText(text, subtext);
    
    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    toggleIndex++;
}

void Module::addKeybind(std::string text, std::string subtext, std::string& keybind) {
    float elementX = Constraints::PercentageConstraint(0.13f, "right");
    float y = Constraints::PercentageConstraint(0.08, "top") + padding;

    FlarialGUI::KeybindSelector(keybindIndex, elementX, y, keybind);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    keybindIndex++;
}

void Module::loadDefaults() {
    settings.reset();
    setup();
}

void Module::saveSettings() {
    if (isScripting()) {
        settingspath = fmt::format("{}\\Scripts\\Configs\\{}.flarial", Utils::getClientPath(), name);
    } else if (Client::settings.getSettingByName<std::string>("currentConfig")->value != "default") {
        settingspath = fmt::format("{}\\{}\\{}.flarial", Utils::getConfigsPath(), Client::settings.getSettingByName<std::string>("currentConfig")->value, name);
    } else {
        settingspath = fmt::format("{}\\{}.flarial", Utils::getConfigsPath(), name);
    }
    checkSettingsFile();

    try {
        std::ofstream outputFile(settingspath);
        if (!outputFile.is_open()) {
            Logger::error("Failed to open file for writing: {}", settingspath.string());
            return;
        }
        outputFile << settings.ToJson();
        outputFile.close();
    } catch (const std::exception& e) {
        Logger::error("An error occurred while saving settings: {}", e.what());
    }
}

void Module::loadSettings() {
    if (isScripting()) {
        settingspath = fmt::format("{}\\Scripts\\Configs\\{}.flarial", Utils::getClientPath(), name);
    } else if (Client::settings.getSettingByName<std::string>("currentConfig")->value != "default") {
        settingspath = fmt::format("{}\\{}\\{}.flarial", Utils::getConfigsPath(), Client::settings.getSettingByName<std::string>("currentConfig")->value, name);
    } else {
        settingspath = fmt::format("{}\\{}.flarial", Utils::getConfigsPath(), name);
    }
    checkSettingsFile();

    std::ifstream inputFile(settingspath);
    if (!inputFile.is_open()) {
        Logger::error("Failed to open file: {}", settingspath.string());
        return;
    }

    std::stringstream ss;
    ss << inputFile.rdbuf();
    inputFile.close();

    if (!ss.str().empty() && ss.str() != "null") {
        settings.FromJson(ss.str());
    } else {
        this->loadDefaults();
    }

    totalKeybinds = 0;
    totalWaypoints = 0;
    totalmaps = 0;

    for (const auto& settingPair : settings.settings) {
        const std::string& name = settingPair.first;
        if (name.find("keybind") != std::string::npos) {
            ++totalKeybinds;
        } else if (name.find("waypoint") != std::string::npos) {
            ++totalWaypoints;
        }
        else if (name.find("map-") != std::string::npos) {
            ++totalmaps;
        }
    }
}

void Module::checkSettingsFile() {
    if (!std::filesystem::exists(settingspath)) {
        std::filesystem::create_directories(settingspath.parent_path());
        std::ofstream outputFile(settingspath);
        if (!outputFile.is_open()) {
            Logger::error("Failed to create file: {}", settingspath.string());
        }
        outputFile.close();
    }
}

void Module::toggle() {
    enabledState = !enabledState;
}

void Module::setup() {
    if (!isScripting())  defaultConfig();
    Module::defaultConfig();
    keybindActions.push_back([this] (std::vector<std::any> args)-> std::any {
        this->active = !this->active;
        return {};
    });

    onSetup();
    // TODO: might call on enable twice

    if (settings.getSettingByName<bool>("enabled")->value)
        onEnable();
}

void Module::onSetup() { }

// TODO: rename to Enable/Disable?
void Module::onEnable() {
    enabledState = true;
    if (settings.getSettingByName<bool>("enabled"))
    settings.getSettingByName<bool>("enabled")->value = true;
    saveSettings();
}

void Module::onDisable() {
    enabledState = false;
    active = false;
    if (!terminating) {
        if (settings.getSettingByName<bool>("enabled"))
        settings.getSettingByName<bool>("enabled")->value = false;
    }
    saveSettings();
}

void Module::terminate() {
    terminating = true;
    onDisable();
    active = false;
}

// TODO: find all getSettingByName<bool>("enabled")->value and replace!!!
bool Module::isEnabled() {
    if (isScripting() && !ScriptManager::initialized) return false;
    if(!settings.getSettingByName<bool>("enabled")) return false;
    return settings.getSettingByName<bool>("enabled")->value;
}

void Module::setEnabled(bool enabled) {
    if (!settings.getSettingByName<bool>("enabled")) return;

    settings.getSettingByName<bool>("enabled")->value = enabled;
    enabledState = enabled;
}

void Module::setKeybind(const std::string& newKeybind) {
    auto key = settings.getSettingByName<std::string>("keybind");
    if (key == nullptr)
        settings.addSetting("keybind", newKeybind);
}

std::string& Module::getKeybind(const int keybindCount) {
    std::string count;
    if(keybindCount > 0) count = "-" + FlarialGUI::cached_to_string(keybindCount);
    auto key = settings.getSettingByName<std::string>("keybind" + count);
    if (key == nullptr)
        settings.addSetting("keybind", defaultKeybind);
    return key->value;
}

void Module::defaultConfig() {

    getKeybind();

    if (settings.getSettingByName<bool>("enabled") == nullptr) {
        if (name == "Zoom" or name == "Tab List") {
            settings.addSetting("enabled", true);
        }
        else {
            settings.addSetting("enabled", false);
        }
    }

    if (settings.getSettingByName<bool>("favorite") == nullptr) {
        settings.addSetting("favorite", false);
    }

    if (settings.getSettingByName<float>("percentageX") == nullptr) {
        settings.addSetting("percentageX", 0.0f);
        settings.addSetting("percentageY", 0.0f);
    }

    if (settings.getSettingByName<float>("textscale") == nullptr) {
        settings.addSetting("textscale", 1.0f);
    }

    if (settings.getSettingByName<bool>("border") == nullptr) {
        settings.addSetting("border", false);
        settings.addSetting("borderWidth", 1.0f);
    }

    if (settings.getSettingByName<bool>("reversepaddingx") == nullptr)
        settings.addSetting("reversepaddingx", false);
    if (settings.getSettingByName<bool>("reversepaddingy") == nullptr)
        settings.addSetting("reversepaddingy", false);
    if (settings.getSettingByName<float>("padx") == nullptr) settings.addSetting("padx", 0.0f);
    if (settings.getSettingByName<float>("pady") == nullptr) settings.addSetting("pady", 0.0f);
    if (settings.getSettingByName<float>("rectwidth") == nullptr) settings.addSetting("rectwidth", 1.0f);
    if (settings.getSettingByName<float>("rectheight") == nullptr) settings.addSetting("rectheight", 1.0f);
    if (settings.getSettingByName<bool>("responsivewidth") == nullptr)
        settings.addSetting("responsivewidth", false);
    if (settings.getSettingByName<std::string>("textalignment") == nullptr)
        settings.addSetting("textalignment", (std::string)"Center");

    if (settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 32.0f);

    if (settings.getSettingByName<std::string>("bgColor") == nullptr) {
        settings.addSetting("bgColor", (std::string)"000000");
        settings.addSetting("textColor", (std::string)"fafafa");
        settings.addSetting("borderColor", (std::string)"000000");
    }   

    
    
        if (settings.getSettingByName<std::string>("textShadow") == nullptr) {
        settings.addSetting("textShadow", false);
        settings.addSetting("rectShadow", false);
        settings.addSetting("textShadowRGB", false);
        settings.addSetting("rectShadowRGB", false);
        settings.addSetting("textShadowCol", (std::string)"00000");
        settings.addSetting("rectShadowCol", (std::string)"00000");
        settings.addSetting("textShadowOffset", 0.003f);
        settings.addSetting("rectShadowOffset", 0.003f);
        settings.addSetting("rectShadowOpacity", 0.55f);
        settings.addSetting("textShadowOpacity", 0.55f);


    }

    


    if (settings.getSettingByName<float>("bgOpacity") == nullptr) {
        settings.addSetting("bgOpacity", 0.55f);
        settings.addSetting("textOpacity", 1.0f);
        settings.addSetting("borderOpacity", 1.0f);
    }

    if (settings.getSettingByName<bool>("bgRGB") == nullptr) {
        settings.addSetting("bgRGB", false);
        settings.addSetting("textRGB", false);
        settings.addSetting("borderRGB", false);
    }

    if (settings.getSettingByName<float>("uiscale") == nullptr) {

        settings.addSetting("uiscale", 0.65f);
    }

    if (settings.getSettingByName<float>("rotation") == nullptr) {
        settings.addSetting("rotation", 0.0f);
    }

    if (settings.getSettingByName<bool>("BlurEffect") == nullptr) {
        settings.addSetting("BlurEffect", false);
    }

}

bool Module::isKeybind(const std::array<bool, 256>& keys, const int keybindCount) {

    std::string count = "keybind";
    if(keybindCount > 0) count += "-" + FlarialGUI::cached_to_string(keybindCount);
    if(!settings.getSettingByName<std::string>(count)) { return false;}

    std::vector<int> keyCodes = Utils::getStringAsKeys(getKeybind(keybindCount));

    for (int keyCode : keyCodes) {
        if (!keys[keyCode]) {
            return false;
        }
    }
    // TODO: in menu should also mean in Edit Mode, when edit mode all modules set to active(visible? add visible setting (for TAB))
    if (name != "ClickGUI")
        if (FlarialGUI::inMenu) return false;
    if (SDK::getCurrentScreen() == "chat_screen") return false;
    bool allInactive = std::ranges::all_of(FlarialGUI::TextBoxes, [](const auto& pair) {
        return !pair.second.isActive;
    });
    return allInactive;
}

[[nodiscard]] bool Module::isAdditionalKeybind(const std::array<bool, 256>& keys, const std::string& bind) const {
    std::vector<int> keyCodes = Utils::getStringAsKeys(bind);

    for (int keyCode : keyCodes) {
        if (!keys[keyCode]) {
            return false;
        }
    }
    if (name != "ClickGUI")
        if (FlarialGUI::inMenu) return false;
    if (SDK::getCurrentScreen() == "chat_screen") return false;
    bool allInactive = std::ranges::all_of(FlarialGUI::TextBoxes, [](const auto& pair) {
        return !pair.second.isActive; // Access the value in the map
    });
    // All keys in the keybind are being held down
    return allInactive;
}

bool Module::isKeyPartOfKeybind(int keyCode, const int keybindCount) {
    std::vector<int> keyCodes = Utils::getStringAsKeys(getKeybind(keybindCount));
    return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
}

bool Module::isKeyPartOfAdditionalKeybind(int keyCode, const std::string& bind) {
    std::vector<int> keyCodes = Utils::getStringAsKeys(bind);
    return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
}

void Module::checkForRightClickAndOpenSettings(float x, float y, float width, float height) {
    if(FlarialGUI::CursorInRect(x, y, width, height) && MC::mouseButton == MouseButton::Right && MC::held ) {
        auto module = ModuleManager::getModule("ClickGUI");
        if(module != nullptr) {

            module->active = true;
            ClickGUI::editmenu = false;
            FlarialGUI::TextBoxes[0].isActive = false;
            ClickGUI::page.type = "settings";
            ClickGUI::page.module = this->name;
            ClickGUI::curr = "settings";
            FlarialGUI::scrollpos = 0;
            FlarialGUI::barscrollpos = 0;
            ClickGUI::accumilatedPos = 0;
            ClickGUI::accumilatedBarPos = 0;
        }
    }
}
