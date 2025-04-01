#pragma once

#include "../Module.hpp"

class ClickData {
public:
    double timestamp;  // Milliseconds since some reference point
    // ... other click data members ...
};

class CPSCounter : public Module {
private:
    static inline std::vector<ClickData> leftClickList;
    static inline std::vector<ClickData> rightClickList;
    static inline bool rightClickHeld;
    static inline bool leftClickHeld;
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastRightClick;
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastLeftClick;
public:
    CPSCounter() : Module("CPS", "Counts your Clicks per second.", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onSetup() override {
        Listen(this, MouseEvent, &CPSCounter::onMouse)
    }

    void onEnable() override {
        Listen(this, RenderEvent, &CPSCounter::onRender)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &CPSCounter::onRender)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<bool>("rightcps") == nullptr) {
            settings.addSetting("rightcps", false);
        }

        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "CPS: {value}");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));


        this->addHeader("Main");
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.0f);
        this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
        this->addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", this->settings.getSettingByName<bool>(
        "BlurEffect")->value);
        this->addConditionalSlider(this->settings.getSettingByName<bool>(
                "border")->value, "Border Thickness", "", this->settings.getSettingByName<float>("borderWidth")->value, 4.f);
        this->addSlider("Rounding", "Rounding of the rectangle", this->settings.getSettingByName<float>("rounding")->value);
        this->addToggle("Right Click CPS", "", this->settings.getSettingByName<bool>("rightcps")->value);
        this->extraPadding();

        this->addHeader("Text");
        this->addTextBox("Format", "", settings.getSettingByName<std::string>("text")->value);
        this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.0f);
        this->addDropdown("Text Alignment", "",  std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);
        this->addColorPicker("Color", "Text Color", settings.getSettingByName<std::string>("textColor")->value,
                             settings.getSettingByName<float>("textOpacity")->value,
                             settings.getSettingByName<bool>("textRGB")->value);
        this->addToggle("Text Shadow", "Displays a shadow under the text", settings.getSettingByName<bool>("textShadow")->value);
        this->addColorPicker("Shadow Color", "Text Shadow Color", settings.getSettingByName<std::string>("textShadowCol")->value,
            settings.getSettingByName<float>("textShadowOpacity")->value,
            settings.getSettingByName<bool>("textShadowRGB")->value);
        this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);

        this->extraPadding();

        this->addHeader("Colors");
        this->addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value,
                             settings.getSettingByName<float>("bgOpacity")->value,
                             settings.getSettingByName<bool>("bgRGB")->value);
        this->addToggle("Background Shadow", "Displays a shadow under the background", settings.getSettingByName<bool>("rectShadow")->value);
        this->addColorPicker("Shadow Color", "Background Shadow Color", settings.getSettingByName<std::string>("rectShadowCol")->value,
            settings.getSettingByName<float>("rectShadowOpacity")->value,
            settings.getSettingByName<bool>("rectShadowRGB")->value);
        this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("rectShadowOffset")->value, 0.02f, 0.001f);

        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value,
                             settings.getSettingByName<float>("borderOpacity")->value,
                             settings.getSettingByName<bool>("borderRGB")->value);        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);

        this->extraPadding();

        this->addHeader("Misc Customizations");

        this->addToggle("Reverse Padding X", "For Text Position",  this->settings.getSettingByName<bool>(
                "reversepaddingx")->value);

        this->addToggle("Reverse Padding Y", "For Text Position",  this->settings.getSettingByName<bool>(
        "reversepaddingy")->value);

        this->addSlider("Padding X", "For Text Position", this->settings.getSettingByName<float>("padx")->value);
        this->addSlider("Padding Y", "For Text Position", this->settings.getSettingByName<float>("pady")->value);

        this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value, 2.f, 0.001f);
        this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value, 2.f, 0.001f);

        this->addToggle("Responsive Rectangle", "Rectangle resizes with text",  this->settings.getSettingByName<bool>(
"responsivewidth")->value);

        this->addSlider("Rotation", "see for yourself!", this->settings.getSettingByName<float>("rotation")->value, 360.f, 0, false);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

static inline double lastLeftAllowed  = 0.0;
static inline double lastRightAllowed = 0.0;

double getCurrentTime() {
    using namespace std::chrono;
    return duration<double>(high_resolution_clock::now().time_since_epoch()).count();
}

void onMouse(MouseEvent& event) {
    auto limiter = ModuleManager::getModule("CPS Limiter");
    if (limiter == nullptr) return;

    double now = getCurrentTime();

    if (event.getButton() == MouseButton::Left) {
        if (!MC::held) {
            leftClickHeld = false;
        } else {
            leftClickHeld = true;

            if (limiter->settings.getSettingByName<bool>("enabled")->value) {
                float leftCpsLimit = limiter->settings.getSettingByName<float>("Left")->value;
                double leftInterval = 1.0 / leftCpsLimit;

                if ((now - lastLeftAllowed) < leftInterval) {
                    event.cancel();
                    return;
                }
                lastLeftAllowed = now;
            }
            AddLeftClick();
        }
    }

    if (event.getButton() == MouseButton::Right) {
        if (!MC::held) {
            rightClickHeld = false;
        } else {
            rightClickHeld = true;

            if (limiter->settings.getSettingByName<bool>("enabled")->value) {
                float rightCpsLimit = limiter->settings.getSettingByName<float>("Right")->value;
                double rightInterval = 1.0 / rightCpsLimit;

                if ((now - lastRightAllowed) < rightInterval) {
                    event.cancel();
                    return;
                }
                lastRightAllowed = now;
            }
            AddRightClick();
        }
    }
}
//
    void onRender(RenderEvent &event) {
        if (this->isEnabled()) {
            if (!this->settings.getSettingByName<bool>("rightcps")->value) {
                std::string leftCPS = FlarialGUI::cached_to_string(GetLeftCPS());
                this->normalRender(1, leftCPS);
            } else {
                std::string leftAndRightCPS = FlarialGUI::cached_to_string(GetLeftCPS()) + " | " + FlarialGUI::cached_to_string(GetRightCPS());
                this->normalRender(1, leftAndRightCPS);
            }

        }
    }

    static void AddLeftClick() {
        ClickData click{};
        click.timestamp = Microtime();
        leftClickList.insert(leftClickList.begin(), click);

        if (leftClickList.size() >= 100) {
            leftClickList.pop_back();
        }
    }

    static void AddRightClick() {
        ClickData click{};
        click.timestamp = Microtime();
        rightClickList.insert(rightClickList.begin(), click);

        if (rightClickList.size() >= 100) {
            rightClickList.pop_back();
        }
    }

    [[nodiscard]] static int GetLeftCPS() {
        if (leftClickList.empty()) {
            return 0;
        }

        double currentMicros = Microtime();
        auto count = std::count_if(leftClickList.begin(), leftClickList.end(), [currentMicros](const ClickData &click) {
            return (currentMicros - click.timestamp <= 1.0);
        });

        return (int) std::round(count);
    }

    [[nodiscard]] static int GetRightCPS() {
        if (rightClickList.empty()) {
            return 0;
        }

        double currentMicros = Microtime();
        auto count = std::count_if(rightClickList.begin(), rightClickList.end(),
                                   [currentMicros](const ClickData &click) {
                                       return (currentMicros - click.timestamp <= 1.0);
                                   });

        return (int) std::round(count);
    }

    [[nodiscard]] static bool GetLeftHeld() {
        return leftClickHeld;
    }

    [[nodiscard]] static bool GetRightHeld() {
        return rightClickHeld;
    }

private:
    [[nodiscard]] static double Microtime() {
        return (double(std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
    }
};