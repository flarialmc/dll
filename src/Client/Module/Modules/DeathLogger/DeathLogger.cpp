#include "DeathLogger.hpp"

#include "Events/EventManager.hpp"
#include "Modules/Waypoints/Waypoints.hpp"

void DeathLogger::onEnable() {
    Listen(this, TickEvent, &DeathLogger::onTick)

    Module::onEnable();
}

void DeathLogger::onDisable() {
    Deafen(this, TickEvent, &DeathLogger::onTick)

    Module::onDisable();
}

void DeathLogger::defaultConfig() {
    Module::defaultConfig("core");
    setDef("deathWaypoints", true);
}

void DeathLogger::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Death Logger");
    addToggle("Death Waypoints", "waypoint settings are managed in the waypoints module.", "deathWaypoints");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

std::string DeathLogger::getRawDimensionName() {
    BlockSource *blocksrc = SDK::clientInstance->getBlockSource();
    if (!blocksrc) return "Unknown dimension";

    Dimension *dimension = blocksrc->getDimension();
    if (!dimension) return "Unknown dimension";

    return dimension->getName();
}

void DeathLogger::onTick(TickEvent& event) {
    if (!SDK::clientInstance->getLocalPlayer()) return;

    LocalPlayer* player = SDK::clientInstance->getLocalPlayer();
    float health = player->getHealth();

    if (death and health > 0) {
        death = false;
        deathPos = Vec3<float>{};
        printed = false;
        waypointed = false;
    };

    if (health == 0 && !death) {
        death = true;
        deathPos = *player->getPosition();
    };

    if (death && getOps<bool>("deathWaypoints") && !waypointed) {
        std::shared_ptr<Waypoints> waypoints = std::dynamic_pointer_cast<Waypoints>(ModuleManager::getModule("Waypoints"));
        int index = waypoints->WaypointList.size();
        while (true) {
            SettingType<std::string>* s = waypoints->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(index));
            if (s == nullptr) break;
            index++;
        }
        std::string col = "FFFFFF";
        if (waypoints->settings.getSettingByName<bool>("randomizeColor") != nullptr) {
            if (waypoints->getOps<bool>("randomizeColor")) col = FlarialGUI::ColorFToHex(D2D1_COLOR_F(Waypoints::random(), Waypoints::random(), Waypoints::random(), 1.f));
        }
        waypoints->addWaypoint(
            index,
            "(death) waypoint-" + FlarialGUI::cached_to_string(index),
            col,
            Vec3{ SDK::clientInstance->getLocalPlayer()->getPosition()->x, SDK::clientInstance->getLocalPlayer()->getPosition()->y - 1, SDK::clientInstance->getLocalPlayer()->getPosition()->z },
            true,
            true,
            false,
            100.0f
        );
        FlarialGUI::Notify("Added death waypoint!");
        waypointed = true;
    }

    if (death && !printed) {
        SDK::clientInstance->getGuiData()->displayClientMessage(std::format(
            "Player died at <{}, {}, {}> in <{}>",
            static_cast<int>(deathPos.x),
            static_cast<int>(deathPos.y),
            static_cast<int>(deathPos.z),
            getRawDimensionName()
        ));
        printed = true;
    }
}
