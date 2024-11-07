#pragma once

#include "../Module.hpp"

struct Waypoint {
    Vec3<float> position;
    D2D1_COLOR_F color;
    int index;
    bool enabled;
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
        Module::onEnable();
    }
    /*std::string count;
      count = "-" + std::to_string(i);
      this->settings.getSettingByName<std::string>("command" + count)->value;
    */
    void addWaypoint(int index, std::string name, std::string color, Vec3<float> position, bool state, bool config) {
        if (config)
        {
            std::string end = "-" + std::to_string(index);
            this->settings.addSetting("waypoint" + end, (std::string)"");
            this->settings.addSetting("color" + end, (std::string)color);
            this->settings.addSetting("x" + end, position.x);
            this->settings.addSetting("y" + end, position.y);
            this->settings.addSetting("z" + end, position.z);
            this->settings.addSetting("state" + end, (bool)state);
            this->settings.setValue("total", this->settings.getSettingByName<int>("total")->value + 1);
            this->saveSettings();
            
            Waypoint wp(position, FlarialGUI::HexToColorF(color), index, state);
            WaypointList[name] = wp;
        }
        else {
            Waypoint wp(position, FlarialGUI::HexToColorF(color), index, state);
            WaypointList[ name ] = wp;
        }
    }
    void onSetup() override {
        for (int i = 0; i < this->settings.getSettingByName<int>("total")->value; ++i) {
            addWaypoint(
                i,
                this->settings.getSettingByName<std::string>("waypoint-" + std::to_string(i))->value,
                this->settings.getSettingByName<std::string>("color-" + std::to_string(i))->value,
                Vec3 {this->settings.getSettingByName<float>("x-" + std::to_string(i))->value, this->settings.getSettingByName<float>("y-" + std::to_string(i))->value, this->settings.getSettingByName<float>("z-" + std::to_string(i))->value},
                this->settings.getSettingByName<bool>("state-" + std::to_string(i))->value,
                false
            );
        }
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &Waypoints::onRender)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<int>("total") == nullptr) settings.addSetting("total", 0);
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
        this->addButton("Add another Waypoint", "Multi-Keybind command support!", "Add", [this] {

            int index = WaypointList.size();
            addWaypoint(
                index,
                "waypoint-" + std::to_string(index),
                "FFFFFF",
                Vec3{ SDK::clientInstance->getLocalPlayer()->getPosition()->x, SDK::clientInstance->getLocalPlayer()->getPosition()->y, SDK::clientInstance->getLocalPlayer()->getPosition()->z },
                true,
                true
            );
            FlarialGUI::Notify("Added! Scroll down for options.");
        });


        for (const auto& pair : WaypointList) {
            std::string name = pair.first;
            Waypoint waypoint = pair.second;
            this->addHeader(name);
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
        for (const auto& pair : WaypointList) {
            std::string name = pair.first;
            Waypoint waypoint = pair.second;
            std::wstring widename = std::wstring(name.begin(), name.end());
            if (Matrix::WorldToScreen(waypoint.position, screen)) {
                FlarialGUI::FlarialTextWithFont(screen.x, screen.y, widename.c_str(), 500, 0, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.215f, "height", true), DWRITE_FONT_WEIGHT_NORMAL, FlarialGUI::rgbColor, true);
            }
        }
    }

};
/*for (int i = 0; i < WaypointList.size(); ++i) {

            std::string header = (i == 0) ? "Waypoint" : "Command " + std::to_string(i);
            std::string commandSettingName = (i == 0) ? "command" : "command-" + std::to_string(i);

            if (settings.getSettingByName<std::string>(commandSettingName) != nullptr) {
                this->addHeader(header);

                this->addKeybind("Command Keybind", "Hold for 2 seconds!", getKeybind(i));

                this->addTextBox(
                    "Command to Send",
                    "No need for /, And there's a spam limit!",
                    settings.getSettingByName<std::string>(commandSettingName)->value
                );
                this->extraPadding();
            }
        }*/