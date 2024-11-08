#pragma once

#include "../Module.hpp"

struct Waypoint {
    Vec3<float> position;
    bool rgb;
    float opacity;
    int index;
    bool state;
};

class Waypoints : public Module {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last_used;
    std::unordered_map<std::string, Waypoint> WaypointList;
public:
    Waypoints() : Module("Waypoints", "Allows you to mark points in your world.", IDR_MAGNIFY_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &Waypoints::onRender)
        Listen(this, KeyEvent, &Waypoints::onKey);
        Module::onEnable();
    }

    void addWaypoint(int index, std::string name, std::string color, Vec3<float> position, bool state, bool config, bool rgb, float opacity) {
        if (config)
        {
            std::string end = "-" + std::to_string(index);
            this->settings.addSetting("waypoint" + end, (std::string)name);
            this->settings.addSetting("color" + end, (std::string)color);
            this->settings.addSetting("x" + end, position.x);
            this->settings.addSetting("y" + end, position.y);
            this->settings.addSetting("z" + end, position.z);
            this->settings.addSetting("state" + end, (bool)state);
            this->settings.addSetting("rgb" + end, (bool)rgb);
            this->settings.addSetting("opacity" + end, (float)opacity);
            this->settings.setValue("total", this->settings.getSettingByName<float>("total")->value + 1);
            this->saveSettings();
            
            Waypoint wp(position, false, 100.0f, index, state);
            WaypointList[name] = wp;
        }
        else {
            Waypoint wp(position, false, 100.0f, index, state);
            WaypointList[ name ] = wp;
        }
    }

    Vec3<float> getPos(int index) {
        return Vec3{ this->settings.getSettingByName<float>("x-" + std::to_string(index))->value, this->settings.getSettingByName<float>("y-" + std::to_string(index))->value, this->settings.getSettingByName<float>("z-" + std::to_string(index))->value };
    }

    void onSetup() override {

        keybindActions.clear();
        keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
            KeyEvent event = std::any_cast<KeyEvent>(args[0]);
            int index = WaypointList.size();
            addWaypoint(
                index,
                "waypoint-" + std::to_string(index),
                "FFFFFF",
                Vec3{ SDK::clientInstance->getLocalPlayer()->getPosition()->x, SDK::clientInstance->getLocalPlayer()->getPosition()->y - 1, SDK::clientInstance->getLocalPlayer()->getPosition()->z },
                true,
                true,
                false,
                100.0f
            );
            FlarialGUI::Notify("Added waypoint!");
            return {};
        });

        if(this->settings.getSettingByName<float>("total")->value > 0)
        for (int i = 0; i < this->settings.getSettingByName<float>("total")->value; i++) {
            if(!this->settings.getSettingByName<std::string>("waypoint-" + std::to_string(i))) continue;
            std::cout << this->settings.getSettingByName<std::string>("waypoint-" + std::to_string(i))->value << std::endl;
            addWaypoint(
                i,
                this->settings.getSettingByName<std::string>("waypoint-" + std::to_string(i))->value,
                this->settings.getSettingByName<std::string>("color-" + std::to_string(i))->value,
                Vec3 {this->settings.getSettingByName<float>("x-" + std::to_string(i))->value, this->settings.getSettingByName<float>("y-" + std::to_string(i))->value, this->settings.getSettingByName<float>("z-" + std::to_string(i))->value},
                this->settings.getSettingByName<bool>("state-" + std::to_string(i))->value,
                false,
                this->settings.getSettingByName<bool>("rgb-" + std::to_string(i))->value,
                this->settings.getSettingByName<float>("opacity-" + std::to_string(i))->value
            );
        }
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &Waypoints::onRender)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("distance") == nullptr) settings.addSetting("distance", 1000.0f);
        if (settings.getSettingByName<float>("total") == nullptr) settings.addSetting("total", 0.0f);
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, 5000, 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Function");
        this->addButton("Add another Waypoint", "", "Add", [this] {

            int index = WaypointList.size();
            addWaypoint(
                index,
                "waypoint-" + std::to_string(index),
                "FFFFFF",
                Vec3{ SDK::clientInstance->getLocalPlayer()->getPosition()->x, SDK::clientInstance->getLocalPlayer()->getPosition()->y - 1, SDK::clientInstance->getLocalPlayer()->getPosition()->z },
                true,
                true,
                false,
                100.0f
            );
            FlarialGUI::Notify("Added! Scroll down for options.");
        });
        this->addKeybind("Add waypoint keybind", "", getKeybind());
        this->addSlider("Distance", "Change until which distance waypoints will be drawn.", this->settings.getSettingByName<float>("distance")->value, 10000.f, 0.f, true);


        for (auto pair : WaypointList) {
            if(!this->settings.getSettingByName<std::string>("waypoint-" + std::to_string(pair.second.index))) continue;
            this->addHeader(this->settings.getSettingByName<std::string>("waypoint-" + std::to_string(pair.second.index))->value);
            this->addToggle("Enabled", "Change if the waypoint should be shown or not.", this->settings.getSettingByName<bool>("state-" + std::to_string(pair.second.index))->value);
            this->addColorPicker("Color", "Change the color of the waypoint.", this->settings.getSettingByName<std::string>("color-" + std::to_string(pair.second.index))->value, this->settings.getSettingByName<float>("opacity-" + std::to_string(pair.second.index))->value, this->settings.getSettingByName<bool>("rgb-" + std::to_string(pair.second.index))->value);
            this->addTextBox("Name", "Change the name of the waypoint.", this->settings.getSettingByName<std::string>("waypoint-" + std::to_string(pair.second.index))->value);
            this->addButton("Delete Waypoint", "", "Delete", [this, index = pair.second.index]() {
                std::string end = "-" + std::to_string(index);
                this->settings.deleteSetting("waypoint" + end);
                this->settings.deleteSetting("color" + end);
                this->settings.deleteSetting("x" + end);
                this->settings.deleteSetting("y" + end);
                this->settings.deleteSetting("z" + end);
                this->settings.deleteSetting("state" + end);
                this->settings.deleteSetting("rgb" + end);
                this->settings.deleteSetting("opacity" + end);
                this->saveSettings();
            });
        }

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onRender(RenderEvent& event) {
        if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
            !SDK::clientInstance->getLocalPlayer()->getLevel())
            return;

        if (FlarialGUI::inMenu || SDK::getCurrentScreen() != "hud_screen") return;

        Vec2<float> screen;

        for (auto pair : WaypointList) {
            if(!this->settings.getSettingByName<bool>("state-" + std::to_string(pair.second.index))) continue;
            if (this->settings.getSettingByName<bool>("state-" + std::to_string(pair.second.index))->value)
            {
                std::string name = this->settings.getSettingByName<std::string>("waypoint-" + std::to_string(pair.second.index))->value;
                Waypoint waypoint = pair.second;

                Vec3<float> waypointPos = getPos(waypoint.index);
                if (Matrix::WorldToScreen(waypointPos, screen)) {

                    Vec3<float> origin = MC::Transform.origin;
                    float distance = waypointPos.dist(origin);
                    name += " (" + std::to_string(int(round(distance))) + "m)";

                    std::wstring widename = std::wstring(name.begin(), name.end());

                    float minFontSize = Constraints::RelativeConstraint(0.09f, "height", true);
                    float maxFontSize = Constraints::RelativeConstraint(0.14f, "height", true);

                    float fontSize = maxFontSize - (maxFontSize - minFontSize) * (distance / this->settings.getSettingByName<float>("distance")->value);
                    fontSize = std::max(minFontSize, std::min(fontSize, maxFontSize));

                    if (distance < this->settings.getSettingByName<float>("distance")->value)
                    {
                        if (this->settings.getSettingByName<bool>("rgb-" + std::to_string(pair.second.index))->value)
                        {
                            FlarialGUI::FlarialTextWithFont(screen.x, screen.y, widename.c_str(), fontSize, 0, DWRITE_TEXT_ALIGNMENT_LEADING, fontSize, DWRITE_FONT_WEIGHT_NORMAL, FlarialGUI::rgbColor, true);
                        }
                        else
                        {
                            FlarialGUI::FlarialTextWithFont(screen.x, screen.y, widename.c_str(), fontSize, 0, DWRITE_TEXT_ALIGNMENT_LEADING, fontSize, DWRITE_FONT_WEIGHT_NORMAL, FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color-" + std::to_string(pair.second.index))->value), true);
                        }
                    }
                }
            }
        }
    }

    void onKey(KeyEvent &event) {

        if (this->isEnabled()) {
            for (int i = 0; i <= totalKeybinds - 1; ++i) {
                if(isKeybind(event.keys) && isKeyPartOfKeybind(event.key))
                keybindActions[i]({ std::any(event) });
            }
        }
    }

};