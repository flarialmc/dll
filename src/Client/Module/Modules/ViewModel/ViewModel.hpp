#pragma once

#include "../Module.hpp"
#include "ViewModelListener.hpp"
#include "../../../Events/EventHandler.hpp"


class ViewModel : public Module {

public:

    ViewModel() : Module("ViewModel", "Allows you to see distant places.", IDR_MAGNIFY_PNG, "C") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerOrderedPriorityListener(new ViewModelListener("ViewModel", this), 2);
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("ViewModel");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("itemfov") == nullptr) settings.addSetting("itemfov", 70.0f);
    }

    void settingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Item FOV", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);


        float value = FlarialGUI::Slider(0, toggleX + FlarialGUI::SettingsTextWidth("Item FOV "),
                                           toggleY, this->settings.getSettingByName<float>("itemfov")->value, 180, 0,
                                           false);

        this->settings.getSettingByName<float>("itemfov")->value = value;

    }
};
