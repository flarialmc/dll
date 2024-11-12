#include "MaterialUtils.hpp"

#include "../../SDK/Client/Render/Tessellator/RenderMaterialGroup.hpp"

static mce::MaterialPtr* uiFillColor = nullptr;
static mce::MaterialPtr* uiTextured = nullptr;
static mce::MaterialPtr* particlesAlpha = nullptr;
static mce::MaterialPtr* uiCrosshair = nullptr;

void MaterialUtils::update() {
    if (uiFillColor == nullptr)
        uiFillColor = mce::RenderMaterialGroup::createUI(HashedString("ui_fill_color"));

    if (uiTextured == nullptr)
        uiTextured = mce::RenderMaterialGroup::createUI(HashedString("ui_textured"));

    if (particlesAlpha == nullptr)
        particlesAlpha = mce::RenderMaterialGroup::createUI(HashedString("particles_alpha"));

    if (uiCrosshair == nullptr)
        uiCrosshair = mce::RenderMaterialGroup::createUI(HashedString("ui_crosshair"));
}

mce::MaterialPtr* MaterialUtils::getUIFillColor() {
    return uiFillColor;
}

mce::MaterialPtr* MaterialUtils::getUITextured() {
    return uiTextured;
}

mce::MaterialPtr* MaterialUtils::getParticlesAlpha() {
    return uiTextured;
}

mce::MaterialPtr* MaterialUtils::getUICrosshair() {
    return uiCrosshair;
}
