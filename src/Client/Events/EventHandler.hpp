#pragma once
#include <vector>
#include <string>
#include "Listener.hpp"
#include "Input/KeyEvent.hpp"
#include "Input/MouseEvent.hpp"
#include "Render/RenderEvent.hpp"

class EventHandler {

public:
    static inline std::vector<Listener*> listeners;
public:
    static void registerListener(Listener *listener);
    static void unregisterListener(std::string name);
public:
    static void onRender(RenderEvent &event);
    static void onKey(KeyEvent &event);
    static void onMouse(MouseEvent &event);

    static void unregisterAll();


};

