#pragma once
#include <vector>
#include <string>
#include "Listener.hpp"
#include "Input/KeyEvent.hpp"
#include "Input/MouseEvent.hpp"
#include "Render/RenderEvent.hpp"
#include "Game/TickEvent.hpp"
#include "Game/AttackEvent.hpp"
#include "Network/PacketEvent.hpp"

class EventHandler {

public:
    static inline std::vector<Listener*> listeners;
public:
    static void registerListener(Listener *listener);
    static void unregisterListener(std::string name);
public:
    static void onRender(RenderEvent &event);
    static void onKey(KeyEvent &event);
    static void onAttack(AttackEvent& event);
    static void onMouse(MouseEvent &event);
    static void onTick(TickEvent &event);
    static void onSetupAndRender(SetupAndRenderEvent &event);

    static void unregisterAll();

    static void onPacketSend(PacketEvent &event);
    static void onPacketReceive(PacketEvent &event);
};