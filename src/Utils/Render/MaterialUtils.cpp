#include "MaterialUtils.hpp"

#include "../../SDK/Client/Render/Tessellator/RenderMaterialGroup.hpp"

static mce::MaterialPtr* uiFillColor = nullptr;
static mce::MaterialPtr* uiTextured = nullptr;

void MaterialUtils::update() {
    if (uiFillColor == nullptr)
        uiFillColor = mce::RenderMaterialGroup::createUI(HashedString("ui_fill_color"));

    if (uiTextured == nullptr)
        uiTextured = mce::RenderMaterialGroup::createUI(HashedString("ui_textured"));
}

mce::MaterialPtr* MaterialUtils::getUIFillColor() {
    return uiFillColor;
}

mce::MaterialPtr* MaterialUtils::getUITextured() {
    return uiTextured;
}
