#include "MaterialUtils.hpp"

#include "../../SDK/Client/Render/Tessellator/RenderMaterialGroup.hpp"

static mce::MaterialPtr* uiFillColor = nullptr;

void MaterialUtils::update() {
    if (uiFillColor == nullptr)
        uiFillColor = mce::RenderMaterialGroup::createUI(HashedString("ui_fill_color"));
}

mce::MaterialPtr* MaterialUtils::getUIFillColor() {
    return uiFillColor;
}
