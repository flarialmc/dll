#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TabListListener.hpp"


class TabList : public Module {

public:


    TabList() : Module("Tab List", "oAnshul==bari", "\\Flarial\\assets\\cloudy.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        EventHandler::registerListener(new TabListListener("TabList", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("TabList");

        Module::onDisable();

    }

    void SettingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        

    }
};

