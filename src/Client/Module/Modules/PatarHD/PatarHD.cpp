#include "PatarHD.hpp"

#include "Events/EventManager.hpp"

PatarHD::PatarHD(): Module("PatarHD", "At this Point it should be called Staff Mod", IDR_SKULL_PNG, "")
{
    
}

void PatarHD::onEnable()
{
    Listen(this, RenderEvent, &PatarHD::onRender)
    Module::onEnable();
    reset();
}

void PatarHD::onDisable()
{
    Deafen(this, RenderEvent, &PatarHD::onRender)
    Module::onDisable();
}

void PatarHD::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("dvdmode", true);
    setDef("xveloc", 1.0f);
    setDef("yveloc", 0.69f);
    setDef("scale", 1.0f);
    setDef("mode", (std::string)"Patar");
    
}

void PatarHD::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("PatarHD");
    addDropdown("Mode", "", std::vector<std::string>{"Patar", "Jqms", "Chyves", "Nikita", "treegfx", "FreezeEngine", "StoneHunter", "beaver"}, "mode", true);
    addSlider("Scale", "", "scale", 5.0F);
    addToggle("DVD Mode", "See for yourself", "dvdmode");
    addConditionalSlider(getOps<bool>("dvdmode"), "X Velocity", "", "xveloc", 25.0f);
    addConditionalSlider(getOps<bool>("dvdmode"), "Y Velocity", "", "yveloc", 25.0f);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void PatarHD::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;
    if (SDK::currentScreen != "hud_screen") return;
    float s = Constraints::RelativeConstraint(0.35, "height", true) * getOps<float>("scale");
    int draw = 165;
    LPCSTR mode = "JPG";
    if (getOps<std::string>("mode") == "Jqms")
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
    }
    else if (getOps<std::string>("mode") == "Chyves") {
        mode = "JPG";
        draw = 216;
    }
    else if (getOps<std::string>("mode") == "treegfx") {
        mode = "JPG";
        draw = 217;
    }
    else if (getOps<std::string>("mode") == "FreezeEngine") {
        mode = "PNG";
        draw = 238;
    }
    else if (getOps<std::string>("mode") == "StoneHunter") {
        mode = "PNG";
        draw = 239;
    }
    else if (getOps<std::string>("mode") == "Nikita")
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
    }
    else if (getOps<std::string>("mode") == "beaver") {
        mode = "PNG";
        draw = 246;
    }
    if (getOps<bool>("dvdmode")) {
        FlarialGUI::image(draw, D2D1::RectF(x, y, x + s, y + s), mode);

        x += getOps<float>("xveloc") * xv;
        y += getOps<float>("yveloc") * yv;

        if (x >= MC::windowSize.x - s) xv = -1;
        if (x < 0) xv = 1;
        if (y >= MC::windowSize.y - s) yv = -1;
        if (y < 0) yv = 1;
    }
    else {
        float s2 = s / 2;
        Vec2<float> center = Vec2<float>{ MC::windowSize.x / 2, MC::windowSize.y / 2 };
        FlarialGUI::image(draw, D2D1::RectF(center.x - s2, center.y - s2, center.x + s2, center.y + s2), mode);
    }
}

int PatarHD::getMs() const
{
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - time);
    return static_cast<int>(duration.count());
}

void PatarHD::reset()
{
    time = std::chrono::steady_clock::now();
}
