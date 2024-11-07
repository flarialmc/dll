#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/EntityEventPacket.hpp"


class OpponentReach : public Module {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
    float opponentReach{};

    std::map<int64_t, Actor*> hitActors;
public:
    OpponentReach() : Module("Opponent Reach", "Shows your opponent's last hit range!",
                       IDR_LIKE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &OpponentReach::onRender)
        Listen(this, PacketEvent, &OpponentReach::onPacketReceive)
        Listen(this, TickEvent, &OpponentReach::onTick)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &OpponentReach::onRender)
        Deafen(this, PacketEvent, &OpponentReach::onPacketReceive)
        Deafen(this, TickEvent, &OpponentReach::onTick)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value}");

        if (settings.getSettingByName<bool>("tryToExcludeTeam") == nullptr)
            settings.addSetting("tryToExcludeTeam", true);

        if (settings.getSettingByName<float>("textscale") == nullptr)
            settings.addSetting("textscale", 0.80f);
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
        this->addToggle("Border", "",  this->settings.getSettingByName<bool>(
                "border")->value);
        this->addConditionalSlider(this->settings.getSettingByName<bool>(
                "border")->value, "Border Thickness", "", this->settings.getSettingByName<float>("borderWidth")->value, 4.f);
        this->addSlider("Rounding", "Rounding of the rectangle", this->settings.getSettingByName<float>("rounding")->value);

        this->extraPadding();

        this->addHeader("Text");
        this->addTextBox("Format", "", settings.getSettingByName<std::string>("text")->value);
        this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.0f);
        this->addDropdown("Text Alignment", "",  std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);
        this->addColorPicker("Color", "Text Color", settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value, settings.getSettingByName<bool>("textRGB")->value);

        this->extraPadding();

        this->addHeader("Colors");
        this->addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value, settings.getSettingByName<bool>("bgRGB")->value);
        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);

        this->extraPadding();

        this->addHeader("Misc Customizations");

        this->addToggle("Reverse Padding X", "For Text Position",  this->settings.getSettingByName<bool>(
                "reversepaddingx")->value);

        this->addToggle("Reverse Padding Y", "For Text Position",  this->settings.getSettingByName<bool>(
        "reversepaddingy")->value);

        this->addSlider("Padding X", "For Text Position", this->settings.getSettingByName<float>("padx")->value);
        this->addSlider("Padding Y", "For Text Position", this->settings.getSettingByName<float>("pady")->value);

        this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value);
        this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value);

        this->addToggle("Responsive Rectangle", "Rectangle resizes with text",  this->settings.getSettingByName<bool>(
"responsivewidth")->value);

        this->addSlider("Rotation", "see for yourself!", this->settings.getSettingByName<float>("rotation")->value, 360.f, 0, false);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void onRender(RenderEvent &event) {
        if (this->isEnabled()) {
            auto delayStr = std::format("{:.2f}", opponentReach);
            this->normalRender(22, delayStr);
        }
    }

    void onPacketReceive(PacketEvent &event) {
        if (event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
            auto packet = (EntityEventPacket *) event.getPacket();
            if (packet->EventID == ActorEvent::Hurt) {

                auto id = packet->RuntimeID;
                if(id == SDK::clientInstance->getLocalPlayer()->getRuntimeIDComponent()->runtimeID) {
                    auto excludeTeam = this->settings.getSettingByName<bool>("tryToExcludeTeam")->value;
                    // we got hit
                    auto player = SDK::clientInstance->getLocalPlayer();
                    auto actors = SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList();
                    // remove non players
                    std::erase_if(actors, [](Actor* actor) {
                        return !actor->hasCategory(ActorCategory::Player) || !actor->isValidAABB();
                    });
                    // remove actors that are too far or on our team
                    std::erase_if(actors, [player, excludeTeam](Actor* actor) {
                        if(actor == player) return true;
                        auto distance = actor->getApproximateReach(player);
                        if(distance > 10.f || distance == 0.f)
                            return true;
                        if(excludeTeam)
                            if(actor->IsOnSameTeam(player))
                                return true;
                        return false;
                    });
                    // Sort the list by distance
                    std::sort(actors.begin(), actors.end(), [&](Actor* a, Actor* b) {
                        return a->getApproximateReach(player) < b->getApproximateReach(player);
                    });

                    // Create a vector to store actors within 5 blocks
                    std::vector<Actor*> nearbyPlayers;

                    // Iterate over the actors and check if they are within 5 blocks
                    for (auto actor : actors) {
                        if (actor->getApproximateReach(player) <= 5.5f) {
                            nearbyPlayers.push_back(actor);
                        }
                    }

                    if(!nearbyPlayers.empty()) {
                        auto likelyOpponent = nearbyPlayers[0];
                        opponentReach = likelyOpponent->getApproximateReach(player);
                        last_hit = std::chrono::high_resolution_clock::now();
                    }
                }
            }
        }
    }

    void onTick(TickEvent &event) {
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
        if (duration.count() >= 15) {
            opponentReach = 0;
        };
    }
};

