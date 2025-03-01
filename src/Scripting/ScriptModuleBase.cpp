#include "ScriptModuleBase.hpp"

#include "ScriptManager.hpp"
#include <SDK/Client/Network/Packet/TextPacket.hpp>

void ScriptModuleBase::onEnable() {
    ScriptManager::executeFunction(moduleLuaState, "onEnable");
    linkedScript->setEnabled(true);
}

void ScriptModuleBase::onDisable() {
    ScriptManager::executeFunction(moduleLuaState, "onDisable");
    linkedScript->setEnabled(false);
}

void ScriptModuleBase::onKey(KeyEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    bool cancelled = linkedScript->registerCancellableEvent("onKey", event.getKey(), static_cast<int>(event.getAction()));
    if (cancelled) event.cancel();
}

void ScriptModuleBase::onMouse(MouseEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized || event.getButton() == 0 || event.getAction() == 0) return;

    bool cancelled = linkedScript->registerCancellableEvent("onMouse", static_cast<int>(event.getButton()), static_cast<int>(event.getAction()));
    if (cancelled) event.cancel();
}

void ScriptModuleBase::onPacketReceive(PacketEvent &event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    bool cancelled = linkedScript->registerCancellableEvent("onPacketReceive", event.getPacket(), static_cast<int>(event.getPacket()->getId()));
    if (cancelled) event.cancel();
}

void ScriptModuleBase::onTick(TickEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    linkedScript->registerEvent("onTick");
}

void ScriptModuleBase::onRender(RenderEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    linkedScript->registerEvent("onRender");
}

void ScriptModuleBase::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    linkedScript->registerEvent("onSetupAndRender");
}

void ScriptModuleBase::onChat(PacketEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (event.getPacket()->getId() != MinecraftPacketIds::Text) return;
    const auto *pkt = reinterpret_cast<TextPacket*>(event.getPacket());

    std::string msg = pkt->message;
    std::string name = pkt->name;
    auto type = pkt->type;
    std::string xuid = pkt->xuid;

    bool cancelled = linkedScript->registerCancellableEvent("onChat", msg, name, static_cast<int>(type), xuid);
    if (cancelled) event.cancel();
}