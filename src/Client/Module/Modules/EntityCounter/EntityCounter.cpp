#include "EntityCounter.hpp"

void EntityCounter::customInit() {
    Listen(this, SetupAndRenderEvent, &EntityCounter::onSetupAndRender);
}

void EntityCounter::customCleanup() {
    Deafen(this, SetupAndRenderEvent, &EntityCounter::onSetupAndRender);
}

void EntityCounter::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled()) return;
    if (SDK::clientInstance->getLocalPlayer()) {
        auto level = SDK::clientInstance->getLocalPlayer()->getLevel();
        if (!level) return;
        entityCount = (int)level->getRuntimeActorList().size();
    }
}

std::string EntityCounter::getDisplayValue() {
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return "";
    return std::format("{}", entityCount);
}
