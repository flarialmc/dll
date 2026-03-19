#pragma once

#include "../../../../Client.hpp"
#include "../../../Manager.hpp"
#include "Events/Listener.hpp"
#include "Events/Input/KeyEvent.hpp"

class F1Listener : public Listener {
    bool hudHidden = false;
    std::string lastScreen;

public:
    void onKey(KeyEvent &event) {
        // Skip if ClickGUI/edit menu is open — Minecraft won't see F1 either
        // (ClickGUI cancels key events), so toggling here would desync.
        if (event.isCancelled()) return;
        auto clickGUI = ModuleManager::getModule("ClickGUI");
        if (clickGUI && clickGUI->active) return;

        // Only toggle on gameplay screens where Minecraft also processes F1.
        // Ignoring pause/inventory/chat prevents our state from desyncing with the game.
        if (event.getKey() == VK_F1 && event.getAction() == ActionType::Pressed) {
            auto screen = SDK::getCurrentScreen();
            if (screen == "hud_screen" || screen == "f1_screen") {
                hudHidden = !hudHidden;
            }
        }
    }

    void onSetTopScreenName(SetTopScreenNameEvent &event) {
        // Sync guard: Minecraft resets HUD visibility on world transitions.
        // Reset our shadow state when entering a new world (start_screen/
        // play_screen → hud_screen) so we don't drift from the game.
        auto layer = event.getLayer();
        if (layer == "hud_screen" &&
            (lastScreen == "start_screen" || lastScreen == "play_screen" ||
             lastScreen == "world_loading_progress_screen")) {
            hudHidden = false;
        }
        lastScreen = layer;

        if (layer != "hud_screen") return;

        if (hudHidden) {
            event.setCustomLayer("f1_screen");
        }
    }

    F1Listener() {
        Listen(this, KeyEvent, &F1Listener::onKey);
        Listen(this, SetTopScreenNameEvent, &F1Listener::onSetTopScreenName);
    }

    ~F1Listener() {
        Deafen(this, KeyEvent, &F1Listener::onKey);
        Deafen(this, SetTopScreenNameEvent, &F1Listener::onSetTopScreenName);
    }
};
