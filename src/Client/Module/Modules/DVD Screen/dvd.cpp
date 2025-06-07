#include "dvd.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"

void DVD::onEnable() {
    Listen(this, RenderEvent, &DVD::onRender)
    Module::onEnable();
}

void DVD::onDisable() {
    Deafen(this, RenderEvent, &DVD::onRender)
    Module::onDisable();
}

void DVD::defaultConfig() {
    Module::defaultConfig("core");
    setDef("xveloc", 1.0f);
    setDef("yveloc", 0.69f);
    setDef("scale", 1.0f);
    Client::SaveSettings();
}

void DVD::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("DVD Screensaver");
    addSlider("Scale", "", "scale");
    addSlider("X Velocity", "", "xveloc");
    addSlider("Y Velocity", "", "yveloc");

    FlarialGUI::UnsetScrollView();

    resetPadding();

}

void DVD::onRender(RenderEvent &event) {
    if (isEnabled() &&
        ClientInstance::getTopScreenName() == "hud_screen") {
        float height = 83 * getOps<float>("scale");
        float width = 187 * getOps<float>("scale");

        FlarialGUI::image(IDR_DVDLOGO_01_PNG - 1 + color,
                          D2D1::RectF(x, y, x + width, y + height));

        x += getOps<float>("xveloc") * xv;
        y += getOps<float>("yveloc") * yv;

        if (x >= MC::windowSize.x - width) {
            xv = -1;
            inc();
        }
        if (x < 0) {
            xv = 1;
            inc();
        }
        if (y >= MC::windowSize.y - height) {
            yv = -1;
            inc();
        }
        if (y < 0) {
            yv = 1;
            inc();
        }
    }
}

void DVD::inc() {
    if (color == 8) color = 1;
    else color++;
}
