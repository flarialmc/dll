#include "ModuleScript.hpp"

#include "ScriptUtils.hpp"
#include "ScriptManager.hpp"
#include <SDK/Client/Network/Packet/TextPacket.hpp>
#include <SDK/Client/Network/Packet/SetTitlePacket.hpp>

void ModuleScript::onEnable() {
    if (const auto& script = linkedScript.lock()) {
        if (script->getState()) {
            ScriptManager::executeFunction(script->getState(), "onEnable");
            Module::onEnable();
        }
    }
}

void ModuleScript::onDisable() {
    if (const auto& script = linkedScript.lock()) {
        if (script->getState()) {
            ScriptManager::executeFunction(script->getState(), "onDisable");
            Module::onDisable();
        }
    }
}

void ModuleScript::onKey(KeyEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (static_cast<int>(event.getAction()) == 1) {
        ScriptUtils::onKeyPressed(event.getKey());
    } else if (static_cast<int>(event.getAction()) == 0) {
        ScriptUtils::onKeyReleased(event.getKey());
    }

    if (const auto& script = linkedScript.lock()) {
        bool cancelled = script->registerCancellableEvent("KeyEvent", event.getKey(), static_cast<int>(event.getAction()));
        if (cancelled) event.cancel();
    }
}

void ModuleScript::onMouse(MouseEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized || event.getButton() == 0 || event.getAction() == 0) return;

    if (const auto& script = linkedScript.lock()) {
        bool cancelled = script->registerCancellableEvent("MouseEvent", static_cast<int>(event.getButton()), static_cast<int>(event.getAction()));
        if (cancelled) event.cancel();
    }
}

void ModuleScript::onPacketReceive(PacketEvent &event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        bool cancel = false;
        auto id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::Text) {
            const auto* tp = reinterpret_cast<TextPacketProxy*>(event.getPacket());
            if (tp) {
                std::string msg = tp->getMessage();
                std::string name = tp->getName();
                int type = static_cast<int>(tp->getType());

                bool ChatReceiveEvent = script->registerCancellableEvent("ChatReceiveEvent", msg, name, type);
                if (ChatReceiveEvent) cancel = true;
            }
        }
        if (id == MinecraftPacketIds::ChangeDimension) {
            bool ChangeDimensionEvent = script->registerCancellableEvent("ChangeDimensionEvent");
            if (ChangeDimensionEvent) cancel = true;
        }
        if (id == MinecraftPacketIds::Login) {
            bool LoginEvent = script->registerCancellableEvent("LoginEvent");
            if (LoginEvent) cancel = true;
        }
        if (id == MinecraftPacketIds::Disconnect) {
            bool DisconnectEvent = script->registerCancellableEvent("DisconnectEvent");
            if (DisconnectEvent) cancel = true;
        }
        if (id == MinecraftPacketIds::ContainerOpen) {
            bool ContainerOpenEvent = script->registerCancellableEvent("ContainerOpenEvent");
            if (ContainerOpenEvent) cancel = true;
        }
        if (id == MinecraftPacketIds::ContainerClose) {
            bool ContainerCloseEvent = script->registerCancellableEvent("ContainerCloseEvent");
            if (ContainerCloseEvent) cancel = true;
        }
        if (id == MinecraftPacketIds::SetTitle) {
            const auto* stp = reinterpret_cast<SetTitlePacket*>(event.getPacket());
            if (stp) {
                std::string text = stp->text;
                int type = stp->type;

                bool SetTitleEvent = script->registerCancellableEvent("SetTitleEvent", text, type);
                if (SetTitleEvent) cancel = true;
            }
        }
        if (id == MinecraftPacketIds::Respawn) {
            bool RespawnEvent = script->registerCancellableEvent("RespawnEvent");
            if (RespawnEvent) cancel = true;
        }
        if (cancel) event.cancel();
    }
}

void ModuleScript::onPacketSend(PacketSendEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        bool cancel = false;

        if (event.getPacket()->getId() == MinecraftPacketIds::Text) {
            const auto *pkt = reinterpret_cast<TextPacketProxy*>(event.getPacket());
            if (pkt) {
                std::string msg = pkt->getMessage();
                std::string name = pkt->getName();
                auto type = pkt->getType();

                bool ChatSendEvent = script->registerCancellableEvent("ChatSendEvent", msg, name, static_cast<int>(type));
                if (ChatSendEvent) cancel = true;
            }
        }
        if (cancel) event.cancel();
    }
}

void ModuleScript::onTick(TickEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        script->registerEvent("TickEvent");
    }
}

void ModuleScript::onRender(RenderEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        script->registerEvent("RenderEvent");
    }
}

void ModuleScript::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        script->registerEvent("SetupAndRenderEvent");
    }
}

void ModuleScript::onAttack(AttackEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        script->registerEvent("AttackEvent");
    }
}

void ModuleScript::onFOV(FOVEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        if (event.getFOV() == 70) return;

        script->registerEvent("FOVEvent", event.getFOV());
    }
}

void ModuleScript::onPerspective(PerspectiveEvent& event) {
    if (!isEnabled() || !ScriptManager::initialized) return;

    if (const auto& script = linkedScript.lock()) {
        script->registerEvent("PerspectiveEvent", static_cast<int>(event.getPerspective()));
    }
}