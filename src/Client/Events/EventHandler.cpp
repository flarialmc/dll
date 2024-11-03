#pragma once

#include "EventHandler.hpp"
#include "Render/RenderEvent.hpp"
#include "Network/PacketEvent.hpp"
#include "Render/RenderPotionHUDEvent.hpp"
#include "../Client.hpp"

// TODO: rewrite event stuff
 void EventHandler::registerListener(Listener *listener) {
    Logger::debug("[EventHandler] Added listener: " + listener->name);
    listeners.push_back(listener);
}

void EventHandler::registerPriorityListener(Listener *listener) {
    Logger::debug("[EventHandler] Added listener: " + listener->name);
    listeners.insert(listeners.begin(),listener);
}

void EventHandler::registerOrderedPriorityListener(Listener *listener, int position) {
    Logger::debug("[EventHandler] Added listener: " + listener->name);
    auto it = listeners.begin();
    std::advance(it, position);
    listeners.insert(it, listener);
}

void EventHandler::unregisterListener(std::string name) {

    auto it = std::find_if(listeners.begin(), listeners.end(),
                           [&](const auto &listener) {
        if(listener->name == name) {
            Logger::debug("[EventHandler] Deleted listener: " + name);
            listener->onUnregister();
            return true;
        } else { return false; }
    });
    if (it != listeners.end()) {
        listeners.erase(it);
    }

}

void EventHandler::unregisterAll() {

    for (Listener *&listener: listeners) {

        listener->onUnregister();

    }

    listeners.clear();

}

void EventHandler::onTick(TickEvent &event) {

    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {

        listener->onTick(event);

    }

}

void EventHandler::onPacketSend(PacketEvent &event) {

    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {

        listener->onPacketSend(event);

    }

}

void EventHandler::onPacketReceive(PacketEvent &event) {

    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {

        listener->onPacketReceive(event);

    }

}

void EventHandler::onKey(KeyEvent &event) {

    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
    auto currentScreen = SDK::getCurrentScreen();
    if (currentScreen != "hud_screen" &&
        currentScreen != "f1_screen" &&
        currentScreen != "zoom_screen"&&
        currentScreen != "start_screen"&&
        currentScreen != "play_screen") return;

    for (Listener *&listener: listeners) {
        listener->onKey(event);
    }

}

void EventHandler::onAttack(AttackEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {

        listener->onAttack(event);

    }

}

void EventHandler::onMouse(MouseEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {

        listener->onMouse(event);

    }

}

void EventHandler::onRender(RenderEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {

        listener->onRender(event);

    }

}

void EventHandler::onSetupAndRender(SetupAndRenderEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {
        listener->onSetupAndRender(event);
    }
}

void EventHandler::onGetViewPerspective(PerspectiveEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {
        listener->onGetViewPerspective(event);
    }
}

void EventHandler::onGetSensitivity(SensitivityEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
//    if (SDK::currentScreen != "hud_screen" && SDK::currentScreen != "zoom_screen" && SDK::currentScreen != "f1_screen") return;

    for (Listener *&listener: listeners) {
        listener->onGetSensitivity(event);
    }
}

void EventHandler::onGetFogColor(FogColorEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {
        listener->onGetFogColor(event);
    }
}

void EventHandler::onGetHurtColor(HurtColorEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {
        listener->onGetHurtColor(event);
    }
}

void EventHandler::onGetGamma(GammaEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;

    for (Listener *&listener: listeners) {
        listener->onGetGamma(event);
    }
}

void EventHandler::onGetFOV(FOVEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
//    if (SDK::currentScreen != "hud_screen" && SDK::currentScreen != "zoom_screen" && SDK::currentScreen != "f1_screen") return;

    for (Listener *&listener: listeners) {
        listener->onGetFOV(event);
    }
}

void EventHandler::onDrawText(DrawTextEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
    for (Listener *&listener: listeners) {
        listener->onDrawText(event);
    }
}

void EventHandler::onDrawImage(DrawImageEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
    for (Listener *&listener: listeners) {
        listener->onDrawImage(event);
    }
}

void EventHandler::onSetTopScreenName(SetTopScreenNameEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
    for (Listener *&listener: listeners) {
        listener->onSetTopScreenName(event);
    }
}

void EventHandler::onRaknetTick(RaknetTickEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
    for (Listener *&listener: listeners) {
        listener->onRaknetTick(event);
    }
}

void EventHandler::onGetTexture(GetTextureEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
    for (Listener *&listener: listeners) {
        listener->onGetTexture(event);
    }
}

void EventHandler::onRenderPotionHUD(RenderPotionHUDEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
    for (Listener *&listener: listeners) {
        listener->onRenderPotionHUD(event);
    }
}

void EventHandler::onUpdatePlayer(UpdatePlayerEvent &event) {
    if (Client::disable) return;
    if (!ModuleManager::initialized) return;
    for (Listener *&listener: listeners) {
        listener->onUpdatePlayer(event);
    }
}
