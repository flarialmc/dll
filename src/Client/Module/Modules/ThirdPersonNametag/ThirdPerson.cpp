#include "ThirdPerson.hpp"
#include <GUI/Engine/EngineImGui.hpp>

#include "Client.hpp"


void ThirdPerson::defaultConfig() {
    Module::defaultConfig("core");
    setDef("nameTagText", (std::string)"ffffff", 1.f, false);
    setDef("nameTagBg", (std::string)"000000", 0.25f, false);
}

void ThirdPerson::onEnable() {
    patch();
    Listen(this, PerspectiveEvent, &ThirdPerson::onGetViewPerspective)
    Listen(this, DrawNameTagEvent, &ThirdPerson::onDrawNameTag)
    Module::onEnable();
}

void ThirdPerson::onDisable() {
    Deafen(this, PerspectiveEvent, &ThirdPerson::onGetViewPerspective)
    Deafen(this, DrawNameTagEvent, &ThirdPerson::onDrawNameTag)
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

void ThirdPerson::onDrawNameTag(DrawNameTagEvent &event) {
    D2D_COLOR_F textCol = getColor("nameTagText");
    D2D_COLOR_F bgCol = getColor("nameTagBg");

    event.getTagRenderObject()->tagColor = MCCColor(bgCol.r, bgCol.g, bgCol.b, bgCol.a);
    event.getTagRenderObject()->textColor = MCCColor(textCol.r, textCol.g, textCol.b, textCol.a);
}


void ThirdPerson::settingsRender(float settingsOffset) {
	initSettingsPage();

	addHeader("Nametag");

    addColorPicker("Text Color", "", "nameTagText");
    addColorPicker("Background Color", "", "nameTagBg");

	FlarialGUI::UnsetScrollView();

	resetPadding();
}
