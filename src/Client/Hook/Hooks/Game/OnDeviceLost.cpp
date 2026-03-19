#include "OnDeviceLost.hpp"
#include "Client.hpp"
#include "../../../Module/Manager.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"

void OnDeviceLostHook::callback(void* game) {
    // If ClickGUI is active, skip the original call to prevent the game from pausing
    if (clickgui && clickgui->active) {
        return; // Don't call original - game stays active
    }

    deviceLostOriginal(game);
}

void OnDeviceLostHook::enableHook() {
    this->autoHook((void*)callback, (void**)&deviceLostOriginal);
}
