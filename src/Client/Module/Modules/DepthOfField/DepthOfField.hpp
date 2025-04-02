#pragma once

#include "../Module.hpp"
#include "DepthMap.hpp"
#include "../../../Hook/Hooks/Render/UnderUIHooks.hpp"

class DepthOfField : public Module {

public:


    DepthOfField() : Module("Depth Of Field", "No need to hold down your sneak key.", IDR_SLOWLY_PNG, "SHIFT") {

        Module::setup();

    };

    void onEnable() override {
        Listen(this, KeyEvent, &DepthOfField::onKey)
            Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, KeyEvent, &DepthOfField::onKey)
            Module::onDisable();
    }

    void defaultConfig() override {
        Module::defaultConfig();

        if (settings.getSettingByName<bool>("status") == nullptr) settings.addSetting("status", false);

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);

        if (settings.getSettingByName<bool>("always") == nullptr) {
            settings.addSetting("always", false);
        }
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Keybind", "Hold for 2 seconds!", getKeybind());

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }

    bool toggleSneaking = false;
    bool toggled = false;

    void onKey(KeyEvent& event) { // TODO: it lets sneak key up through (flickers sneak once)

        if (!toggled) {
            if (!DepthMapGenerator::Initialize()) {
                Logger::error("Error initializing depth map gen");
            }
            toggled = true;
        }

        if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
            ID3D11Texture2D* depthTexture = nullptr;
            UnderUIHooks::savedDepthStencilView->GetResource(reinterpret_cast<ID3D11Resource**>(&depthTexture));
            if(!DepthMapGenerator::GenerateDepthMap(depthTexture)) Logger::error("Error depth stuff");
            
            depthTexture->Release();
        }
    };
};

