#pragma once
#include "EventHandler.hpp"
#include "Render/RenderEvent.hpp"
#include "../../SDK/SDK.hpp"
#include "Network/PacketEvent.hpp"
#include "../Client.hpp"

void EventHandler::registerListener(Listener *listener) {
    listeners.push_back(listener);
}

void EventHandler::unregisterListener(std::string name) {

    auto it = std::find_if(listeners.begin(), listeners.end(),
                           [&](const auto &listener) { return listener->name == name; });
    if (it != listeners.end()) {
        listeners.erase(it);
    }

}

void EventHandler::unregisterAll() {

    listeners.clear();

}

void EventHandler::onTick(TickEvent &event) {

    if (Client::disable) return;

    for (Listener* &listener: listeners) {

        listener->onLocalTick(event);

    }

}

void EventHandler::onPacketSend(PacketEvent &event) {

    if (Client::disable) return;

    for (Listener* &listener: listeners) {

        listener->onPacketSend(event);

    }

}

void EventHandler::onPacketReceive(PacketEvent &event) {

    for (Listener* &listener: listeners) {

        listener->onPacketReceive(event);

    }

}

void EventHandler::onKey(KeyEvent &event) {

    if (Client::disable) return;

    for (Listener* &listener: listeners) {

        listener->onKey(event);

    }
}

void EventHandler::onAttack(AttackEvent& event) {

    if (Client::disable) return;

    for (Listener*& listener : listeners) {
        
        listener->onAttack(event);

    }


}

void EventHandler::onMouse(MouseEvent &event) {

    if (Client::disable) return;

    for (Listener* &listener: listeners) {

        listener->onMouse(event);

    }
}

void EventHandler::onRender(RenderEvent &event) {

    if (Client::disable) return;

    for (Listener* &listener: listeners) {

        listener->onRender(event);

        FlarialGUI::displayToolTips();
    }
}

void EventHandler::onSetupAndRender(SetupAndRenderEvent &event) {

    if (Client::disable) return;

    for (Listener* &listener: listeners) {
        listener->onSetupAndRender(event);
    }

}
