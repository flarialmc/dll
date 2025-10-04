#include "ParticleMultiplier.hpp"
#include <GUI/Engine/EngineImGui.hpp>
#include "Events/EventManager.hpp"
#include "GUI/Engine/EngineImGui.hpp"
#include "Hook/Hooks/Game/PacketHooks.hpp"
#include "SDK/Client/Network/Packet/AnimatePacket.hpp"
#include "SDK/Client/Actor/LocalPlayer.hpp"
#include <SDK/Client/Actor/Components/RuntimeIDComponent.hpp>


void ParticleMultiplier::onEnable() {
    Listen(this, RenderEvent, &ParticleMultiplier::onRender)
    Listen(this, PacketEvent, &ParticleMultiplier::onPacketReceive)
    Listen(this, AttackEvent, &ParticleMultiplier::onAttack)
    Module::onEnable();

}

void* stolenDispatcher = nullptr;
void* stolenIdentifier = nullptr;
void* stolenCallback = nullptr;

void ParticleMultiplier::onDisable() {
    Deafen(this, RenderEvent, &ParticleMultiplier::onRender)
    Deafen(this, PacketEvent, &ParticleMultiplier::onPacketReceive)
    Deafen(this, AttackEvent, &ParticleMultiplier::onAttack)
    Module::onDisable();

    stolenDispatcher = nullptr;
    stolenIdentifier = nullptr;
    stolenCallback = nullptr;
}

void ParticleMultiplier::defaultConfig() {
    Module::defaultConfig("core");
    setDef("intensity", 50.f);
    setDef("normalhitcrit", true);
}

void ParticleMultiplier::settingsRender(float settingsOffset) {
    initSettingsPage();

    addSlider("Intensity", "", "intensity", 500.f);
    addToggle("Normal Hit Crit", "Multiply crit particles on normal hits too.", "normalhitcrit");
    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ParticleMultiplier::onRender(RenderEvent &event) {
    if (SDK::currentScreen != "hud_screen")
    {
        stolenDispatcher = nullptr;
        stolenIdentifier = nullptr;
        stolenCallback = nullptr;
    }
}

void ParticleMultiplier::onPacketReceive(PacketEvent& event)
{
    if (!this->isEnabled()) return;

    Logger::debug(SDK::currentScreen);


    MinecraftPacketIds id = event.getPacket()->getId();

    if (!stolenDispatcher)
    {
        stolenDispatcher = event.getPacketHandlerDispatcher();
    }

    if (!stolenIdentifier)
    {
        stolenIdentifier = event.getNetworkIdentifier();
    }

    if (!stolenCallback)
    {
        stolenCallback = event.getNetEventCallback();
    }

    if (id == MinecraftPacketIds::Animate)
    {
        for (int i = 0; i < getOps<float>("intensity"); i++)
        {
            SendPacketHook::receivePacketAnimateOriginal(event.getPacketHandlerDispatcher(), event.getNetworkIdentifier(), event.getNetEventCallback(), event.getPacketShared());
        }
    }

}

void ParticleMultiplier::onAttack(AttackEvent& event)
{

    if (!stolenDispatcher || !stolenIdentifier || !stolenCallback || !isEnabled() || !getOps<bool>("normalhitcrit")) return;
    std::shared_ptr<Packet> packet = SDK::createPacket(static_cast<int>(MinecraftPacketIds::Animate));
    std::shared_ptr<AnimatePacket> pkt = std::static_pointer_cast<AnimatePacket>(packet);

    pkt->mAction = AnimatePacket::Action::CriticalHit;
    pkt->mRuntimeID = event.getActor()->getRuntimeIDComponent()->runtimeID;

    for (int i = 0; i < getOps<float>("intensity"); i++)
    {
        SendPacketHook::receivePacketAnimateOriginal(stolenDispatcher, stolenIdentifier, stolenCallback, pkt);
    }
}


