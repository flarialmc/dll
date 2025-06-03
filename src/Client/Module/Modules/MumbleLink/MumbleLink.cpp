#include "MumbleLink.hpp"

MumbleLink::MumbleLink(): Module("Mumble Link", "Use proximity chat in Flarial with the help of Mumble", IDR_MUMBLE_PNG, "")
{
    Module::setup();
}

void MumbleLink::onEnable()
{
    Module::onEnable();
    std::thread serverthread([&]() {
        while (this->isEnabled()) {
            SendDataToServer();
            Sleep(5000);
        }
    });
    std::thread datathread([&]() {
        while (this->isEnabled()) {
            if (SDK::clientInstance and SDK::clientInstance->getLocalPlayer()) {
                Pos = *SDK::clientInstance->getLocalPlayer()->getPosition();
                auto rot = SDK::clientInstance->getLocalPlayer()->getActorRotationComponent()->rot;
                yaw = rot.x;
                pitch = rot.y;
                PlayerName = SDK::clientInstance->getLocalPlayer()->getPlayerName();
                Context = getOps<std::string>("context").empty() ? SDK::getServerIP() : getOps<std::string>("context");
            }
            else {

            }

            Sleep(20);
        }
    });

    serverthread.detach();
    datathread.detach();

}

void MumbleLink::onDisable()
{
    Module::onDisable();
}

void MumbleLink::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("context", (std::string)"");
}

void MumbleLink::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

    addHeader("Mumble Link");
    addTextBox("Channel/Room", "Keep empty to use the Server IP.", getOps<std::string>("context"));

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));
    FlarialGUI::UnsetScrollView();

    resetPadding();
}
