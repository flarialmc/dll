#pragma once

#include "../Module.hpp"


class PatarHD : public Module {
private:
    float x = 0;
    float y = 0;
    float xv = 1;
    float yv = 1;
    int pic = 0;
    int lastTime = 0;
    std::chrono::steady_clock::time_point time;
public:

    PatarHD() : Module("PatarHD", "At this Point it should be called Staff Mod", IDR_SKULL_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &PatarHD::onRender)
        Module::onEnable();
        reset();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &PatarHD::onRender)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<bool>("dvdmode") == nullptr) settings.addSetting("dvdmode", true);
        if (settings.getSettingByName<float>("xveloc") == nullptr) settings.addSetting("xveloc", 1.0f);
        if (settings.getSettingByName<float>("yveloc") == nullptr) settings.addSetting("yveloc", 0.69f);
        if (settings.getSettingByName<float>("scale") == nullptr) settings.addSetting("scale", 1.0f);
        if (settings.getSettingByName<std::string>("mode") == nullptr) settings.addSetting("mode", (std::string)"Patar");
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, 5000, 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("General");

        this->addToggle("DVD Mode", "See for yourself", this->settings.getSettingByName<bool>("dvdmode")->value);
        this->addDropdown("Mode", "", std::vector<std::string>{"Patar", "Jqms", "Chyves", "Nikita", "treegfx", "FreezeEngine", "StoneHunter", "beaver"}, this->settings.getSettingByName<std::string>("mode")->value);
        this->addSlider("Scale", "", this->settings.getSettingByName<float>("scale")->value, 5.0F);
        if (this->settings.getSettingByName<bool>("dvdmode")->value) {
            this->addHeader("DVD Mode");
            this->addSlider("X Velocity", "", this->settings.getSettingByName<float>("xveloc")->value, 25.0f);
            this->addSlider("Y Velocity", "", this->settings.getSettingByName<float>("yveloc")->value, 25.0f);
        }

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void onRender(RenderEvent &event) {
        if (SDK::currentScreen != "hud_screen") return;
        float s = Constraints::RelativeConstraint(0.35, "height", true) * this->settings.getSettingByName<float>("scale")->value;
        int draw = 165;
        LPCSTR mode = "JPG";
        if (this->settings.getSettingByName<std::string>("mode")->value == "Jqms")
        {
            mode = "JPG";
            if (getMs() >= 70) {
                reset();
                if (pic == 31) {
                    pic = 0;
                }
                else
                {
                    pic++;
                }
            }
            
            draw = 184 + pic;
        } else if (this->settings.getSettingByName<std::string>("mode")->value == "Chyves") {
            mode = "JPG";
            draw = 216;
        } else if (this->settings.getSettingByName<std::string>("mode")->value == "treegfx") {
            mode = "JPG";
            draw = 217;
        }else if (this->settings.getSettingByName<std::string>("mode")->value == "FreezeEngine") {
            mode = "PNG";
            draw = 238;
        }else if (this->settings.getSettingByName<std::string>("mode")->value == "StoneHunter") {
            mode = "PNG";
            draw = 239;
        } else if (this->settings.getSettingByName<std::string>("mode")->value == "Nikita")
        {
            mode = "PNG";
            if (getMs() >= 30) {
                reset();
                if (pic == 14) {
                    pic = 0;
                }
                else
                {
                    pic++;
                }
            }
            draw = 218 + pic;
        }else if (this->settings.getSettingByName<std::string>("mode")->value == "beaver") {
            mode = "PNG";
            draw = 246;
        }
        if (this->settings.getSettingByName<bool>("dvdmode")->value) {
            FlarialGUI::image(draw, D2D1::RectF(x, y, x + s, y + s), mode);

            x += this->settings.getSettingByName<float>("xveloc")->value * xv;
            y += this->settings.getSettingByName<float>("yveloc")->value * yv;

            if (x >= MC::windowSize.x - s) xv = -1;
            if (x < 0) xv = 1;
            if (y >= MC::windowSize.y - s) yv = -1;
            if (y < 0) yv = 1;
        } else {
            float s2 = s/2 ;
            Vec2<float> center = Vec2<float>{ MC::windowSize.x / 2, MC::windowSize.y / 2 };
            FlarialGUI::image(draw, D2D1::RectF(center.x - s2, center.y - s2, center.x + s2, center.y + s2), mode);
        }
    }

    int getMs() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - time);
        return static_cast<int>(duration.count());
    }
    void reset() {
        time = std::chrono::steady_clock::now();
    }
};