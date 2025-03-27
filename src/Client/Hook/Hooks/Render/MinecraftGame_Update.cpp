#include "MinecraftGame_Update.hpp"
#include "../../../GUI/Engine/Engine.hpp"

void* MinecraftGame_Update::MinecraftGame_UpdateDetour(void* _this) {
    Logger::debug("GamesUpdate : {}", MC::frames);

    FlarialGUI::RoundedRect(true, 0, 0, IM_COL32_WHITE, 500, 500);

    return funcOriginal(_this);
}

void MinecraftGame_Update::enableHook() {
    this->autoHook((void*)MinecraftGame_UpdateDetour, (void**)&funcOriginal);
}