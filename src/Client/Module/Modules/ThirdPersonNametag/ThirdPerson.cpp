#include "ThirdPerson.hpp"

#include "Client.hpp"


void ThirdPerson::defaultConfig() {
    Module::defaultConfig("core");
}

void ThirdPerson::onEnable() {
    patch();
    Listen(this, PerspectiveEvent, &ThirdPerson::onGetViewPerspective)
    Module::onEnable();
}

void ThirdPerson::onDisable() {
    Deafen(this, PerspectiveEvent, &ThirdPerson::onGetViewPerspective)
    unpatch();
    Module::onDisable();
}

void ThirdPerson::patch() {
    Memory::nopBytes((void *) address, 6);
    patched = true;
}

void ThirdPerson::unpatch() {
    Memory::patchBytes((void *) address, original.data(), original.size());
    patched = false;
}

void ThirdPerson::onGetViewPerspective(PerspectiveEvent &event) {
    if (auto sl = ModuleManager::getModule("SnapLook"); sl && sl->active) {
        if (!patched) patch();
    } else if (event.getPerspective() == Perspective::FirstPerson && patched) unpatch();
    else if (event.getPerspective() != Perspective::FirstPerson && !patched) patch();
}