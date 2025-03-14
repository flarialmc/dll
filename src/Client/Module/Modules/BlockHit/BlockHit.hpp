#pragma once

#include "../Module.hpp"
#include "../CPS/CPSCounter.hpp"
#include "../../../Events/Game/BobViewEvent.hpp"

class BlockHit : public Module {

public:
    BlockHit() : Module("Block Hit", "Sword Blocking Animation like Java (visual only)",
                      IDR_LIKE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Module::onEnable();
        Listen(this, BobViewEvent, &BlockHit::onBobView);
    }

    void onDisable() override {
        Deafen(this, BobViewEvent, &BlockHit::onBobView);
        Module::onDisable();
    }

    void onBobView(BobViewEvent& event) {

        auto& matrix = event.matrix;
        auto lp = SDK::clientInstance->getLocalPlayer();

        if(lp != nullptr) {

            auto itemStack = lp->getSupplies()->getInventory()->getItem(lp->getSupplies()->getSelectedSlot());

            if(itemStack->item.get() != nullptr) {
                if (CPSCounter::GetRightHeld() && itemStack->getItem()->name.contains("sword")) {
                    *matrix = glm::translate<float>(*matrix, glm::vec3(0.42222223281, 0.0, -0.16666666269302368));
                    *matrix = glm::translate<float>(*matrix, glm::vec3(-0.1f, 0.15f, -0.2f));
                    *matrix = glm::translate<float>(*matrix, glm::vec3(-0.24F, 0.25f, -0.20F));
                    *matrix = glm::rotate<float>(*matrix, -1.98F, glm::vec3(0.0F, 1.0F, 0.0F));
                    *matrix = glm::rotate<float>(*matrix, 1.30F, glm::vec3(4.0F, 0.0F, 0.0F));
                    *matrix = glm::rotate<float>(*matrix, 60.0F, glm::vec3(0.0F, 1.0F, 0.0F));
                }
            }
        }

    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string) "GG");
    }

    void settingsRender(float settingsOffset) override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};

