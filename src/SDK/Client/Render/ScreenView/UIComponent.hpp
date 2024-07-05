#pragma once

#include "../../../../Utils/Memory/Memory.hpp"

class UIComponent {
public:

};

class MinecraftCustomUIRenderer {
public:
    BUILD_ACCESS(this, float, state, GET_OFFSET("MinecraftCustomUIRenderer::state"));
};

class CustomRenderComponent : public UIComponent {
public:
    BUILD_ACCESS(this, MinecraftCustomUIRenderer*, renderer, GET_OFFSET("CustomRenderComponent::renderer"));
};
