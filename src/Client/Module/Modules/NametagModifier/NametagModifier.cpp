#include "NametagModifier.hpp"

#include "Client.hpp"


void NametagModifier::defaultConfig() {
    Module::defaultConfig("core");
    setDef("nameTagText", (std::string)"ffffff", 1.f, false);
    setDef("nameTagBg", (std::string)"000000", 0.25f, false);
}

void NametagModifier::onEnable() {
    patch();
    Listen(this, PerspectiveEvent, &NametagModifier::onGetViewPerspective)
    Listen(this, DrawNameTagEvent, &NametagModifier::onDrawNameTag)
    Module::onEnable();
}

void NametagModifier::onDisable() {
    Deafen(this, PerspectiveEvent, &NametagModifier::onGetViewPerspective)
    Deafen(this, DrawNameTagEvent, &NametagModifier::onDrawNameTag)
    unpatch();
    Module::onDisable();
}

void NametagModifier::patch() {
    Memory::nopBytes((void *) address, 6);
    patched = true;
}

void NametagModifier::unpatch() {
    Memory::patchBytes((void *) address, original.data(), original.size());
    patched = false;
}

void NametagModifier::onGetViewPerspective(PerspectiveEvent &event) {
    if (auto sl = ModuleManager::getModule("SnapLook"); sl && sl->active) {
        if (!patched) patch();
    } else if (event.getPerspective() == Perspective::FirstPerson && patched) unpatch();
    else if (event.getPerspective() != Perspective::FirstPerson && !patched) patch();
}

void NametagModifier::onDrawNameTag(DrawNameTagEvent &event) {
    D2D_COLOR_F textCol = getColor("nameTagText");
    D2D_COLOR_F bgCol = getColor("nameTagBg");

    event.getTagRenderObject()->tagColor = MCCColor(bgCol.r, bgCol.g, bgCol.b, bgCol.a);
    event.getTagRenderObject()->textColor = MCCColor(textCol.r, textCol.g, textCol.b, textCol.a);
}


void NametagModifier::settingsRender(float settingsOffset) {
	initSettingsPage();

	addHeader("Nametag");

    addColorPicker("Text Color", "", "nameTagText");
    addColorPicker("Background Color", "", "nameTagBg");

	FlarialGUI::UnsetScrollView();

	resetPadding();
}
