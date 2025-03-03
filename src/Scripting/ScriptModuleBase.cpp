#include "ScriptModuleBase.hpp"

#include "ScriptManager.hpp"
#include <SDK/Client/Network/Packet/TextPacket.hpp>

void ScriptModuleBase::onEnable() {
    if (const auto& script = linkedScript.lock()) {
        if (script->getState()) {
            ScriptManager::executeFunction(script->getState(), "onEnable");
        }
    }
    Module::onEnable();
}

void ScriptModuleBase::onDisable() {
    if (const auto& script = linkedScript.lock()) {
        if (script->getState()) {
            ScriptManager::executeFunction(script->getState(), "onDisable");
        }
    }
    Module::onDisable();
}

void ScriptModuleBase::onKey(KeyEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        bool cancelled = script->registerCancellableEvent("onKey", event.getKey(), static_cast<int>(event.getAction()));
        if (cancelled) event.cancel();
    }
}

void ScriptModuleBase::onMouse(MouseEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized || event.getButton() == 0 || event.getAction() == 0) return;

    if (const auto& script = linkedScript.lock()) {
        bool cancelled = script->registerCancellableEvent("onMouse", static_cast<int>(event.getButton()), static_cast<int>(event.getAction()));
        if (cancelled) event.cancel();
    }

}

void ScriptModuleBase::onPacketReceive(PacketEvent &event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        bool cancel = false;

        bool onPacketReceive = script->registerCancellableEvent("onPacketReceive", event.getPacket(), static_cast<int>(event.getPacket()->getId()));
        if (onPacketReceive) cancel = true;

        if (event.getPacket()->getId() == MinecraftPacketIds::Text) {
            const auto *pkt = reinterpret_cast<TextPacket*>(event.getPacket());
            if (pkt) {
                std::string msg = pkt->message;
                std::string name = pkt->name;
                auto type = pkt->type;
                std::string xuid = pkt->xuid;

                bool onChat = script->registerCancellableEvent("onChat", msg, name, static_cast<int>(type), xuid);
                if (onChat) cancel = true;
            }
        }
        if (cancel) event.cancel();
    }
}

void ScriptModuleBase::onTick(TickEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        script->registerEvent("onTick");
    }
}

void ScriptModuleBase::onRender(RenderEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        script->registerEvent("onRender");
    }
}

void ScriptModuleBase::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        script->registerEvent("onSetupAndRender");
    }
}