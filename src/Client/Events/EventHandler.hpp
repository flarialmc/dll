#pragma once

#include <vector>
#include <string>
#include "Listener.hpp"
#include "Input/KeyEvent.hpp"
#include "Input/MouseEvent.hpp"
#include "Render/RenderEvent.hpp"
#include "Game/TickEvent.hpp"
#include "Game/AttackEvent.hpp"
#include "Game/PerspectiveEvent.hpp"
#include "Network/PacketEvent.hpp"
#include "Input/SensitivityEvent.hpp"
#include "Render/FogColorEvent.hpp"
#include "Render/HurtColorEvent.hpp"
#include "Render/GammaEvent.hpp"
#include "Render/GetTextureEvent.hpp"
#include "Render/RenderPotionHUDEvent.hpp"

class EventHandler {

public:
    static inline std::list<Listener *> listeners;
public:
    static void registerListener(Listener *listener);

    static void registerPriorityListener(Listener *listener);

    static void registerOrderedPriorityListener(Listener *listener,  int position);

    static void unregisterListener(std::string name);

public:

    static void onTick(TickEvent &event);

    static void onKey(KeyEvent &event);

    static void onMouse(MouseEvent &event);

    static void onAttack(AttackEvent &event);

    static void onRender(RenderEvent &event);

    static void onPacketSend(PacketEvent &event);

    static void onPacketReceive(PacketEvent &event);

    static void onSetupAndRender(SetupAndRenderEvent &event);

    static void onGetViewPerspective(PerspectiveEvent &event);

    static void onGetSensitivity(SensitivityEvent &event);

    static void onGetFogColor(FogColorEvent &event);

    static void onGetHurtColor(HurtColorEvent &event);

    static void onGetGamma(GammaEvent &event);

    static void onGetFOV(FOVEvent &event);

    static void onDrawText(DrawTextEvent &event);

    static void onDrawImage(DrawImageEvent &event);

    static void onSetTopScreenName(SetTopScreenNameEvent &event);

    static void onRaknetTick(RaknetTickEvent &event);

    static void onGetTexture(GetTextureEvent &event);

    static void onRenderPotionHUD(RenderPotionHUDEvent &event);

    static void onUpdatePlayer(UpdatePlayerEvent &event);

    static void unregisterAll();
};