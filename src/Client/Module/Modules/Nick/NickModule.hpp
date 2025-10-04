#pragma once

#include "../Module.hpp"
#include "Events/Game/RaknetTickEvent.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/DrawTextEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class NickModule : public Module {
private:
    bool enabled = false;
    static inline std::string original2;

public:
    static inline std::string original;
    static inline std::string backupOri;

    static inline std::map<std::string, std::string> textColors = {
        {"White", "§f"},
        {"Black", "§0"},
        {"Netherite", "§j"},
        {"Gray", "§7"},
        {"Iron", "§i"},
        {"Quartz", "§h"},
        {"Dark Gray", "§8"},
        {"Red", "§c"},
        {"Dark Red", "§4"},
        {"Copper", "§n"},
        {"Redstone", "§m"},
        {"Gold", "§6"},
        {"Material Gold", "§p"},
        {"Yellow", "§e"},
        {"Minecoin Gold", "§g"},
        {"Green", "§a"},
        {"Dark Green", "§2"},
        {"Emerald", "§q"},
        {"Diamond", "§s"},
        {"Aqua", "§b"},
        {"Dark Aqua", "§3"},
        {"Blue", "§9"},
        {"Dark Blue", "§1"},
        {"Lapis", "§t"},
        {"Light Purple", "§d"},
        {"Dark Purple", "§5"},
        {"Amethyst", "§u"}
    };

    NickModule(): Module("Nick",
         "Hides your username and replace it with something else.\nWorks everywhere (chat, pause, third person, etc)\nOther people will not be able to see your nick.",
         IDR_ICOGNITO_PNG, "") {

    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onRaknetTick(RaknetTickEvent &event);

    void onDrawText(DrawTextEvent &event);

    void onTick(TickEvent &event);
};
