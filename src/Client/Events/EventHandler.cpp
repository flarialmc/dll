//
// Created by User on 6/7/2023.
//
#pragma once
#include "EventHandler.hpp"
#include "Render/RenderEvent.hpp"

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

void EventHandler::onKey(KeyEvent &event) {

    for (Listener* &listener: listeners) {

        listener->onKey(event);

    }
}

void EventHandler::onMouse(MouseEvent &event) {

    for (Listener* &listener: listeners) {

        listener->onMouse(event);

    }
}

void EventHandler::onRender(RenderEvent &event) {

    for (Listener* &listener: listeners) {

        listener->onRender(event);

    }

}
