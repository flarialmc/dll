#pragma once

#include "Input/KeyEvent.hpp"
#include "Input/MouseEvent.hpp"
#include "Render/RenderEvent.hpp"
#include "Game/TickEvent.hpp"
#include "Game/AttackEvent.hpp"
#include "Game/PerspectiveEvent.hpp"
#include "Render/SetupAndRenderEvent.hpp"
#include "Network/PacketEvent.hpp"
#include "Input/SensitivityEvent.hpp"
#include "Render/FogColorEvent.hpp"
#include "Render/HurtColorEvent.hpp"
#include "Render/GammaEvent.hpp"
#include "Game/FOVEvent.hpp"
#include "Render/DrawTextEvent.hpp"
#include "Render/DrawImageEvent.hpp"
#include "Render/SetTopScreenNameEvent.hpp"
#include "Game/RaknetTickEvent.hpp"
#include "Render/GetTextureEvent.hpp"
#include "Render/RenderPotionHUDEvent.hpp"
#include "Game/UpdatePlayerEvent.hpp"

class Listener {
public:
    std::string name;

    virtual void onTick(TickEvent &event) {};

    virtual void onKey(KeyEvent &event) {};

    virtual void onMouse(MouseEvent &event) {};

    virtual void onAttack(AttackEvent &event) {};

    virtual void onRender(RenderEvent &event) {};

    virtual void onPacketSend(PacketEvent &event) {};

    virtual void onPacketReceive(PacketEvent &event) {};

    virtual void onSetupAndRender(SetupAndRenderEvent &event) {};

    virtual void onGetViewPerspective(PerspectiveEvent &event) {};

    virtual void onGetSensitivity(SensitivityEvent &event) {};

    virtual void onGetFogColor(FogColorEvent &event) {};

    virtual void onGetHurtColor(HurtColorEvent &event) {};

    virtual void onGetGamma(GammaEvent &event) {};

    virtual void onGetFOV(FOVEvent &event) {};

    virtual void onDrawText(DrawTextEvent &event) {};

    virtual void onDrawImage(DrawImageEvent &event) {};

    virtual void onSetTopScreenName(SetTopScreenNameEvent &event) {};

    virtual void onRaknetTick(RaknetTickEvent &event) {};

    virtual void onGetTexture(GetTextureEvent &event) {};

    virtual void onRenderPotionHUD(RenderPotionHUDEvent &event) {};

    virtual void onUpdatePlayer(UpdatePlayerEvent &event) {};

    virtual void onUnregister() {};
};
