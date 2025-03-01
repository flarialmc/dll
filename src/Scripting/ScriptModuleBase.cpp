#include "ScriptModuleBase.hpp"

#include "ScriptManager.hpp"

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