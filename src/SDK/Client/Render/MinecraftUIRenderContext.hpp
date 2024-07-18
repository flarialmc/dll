#pragma once


#include "../Core/ClientInstance.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include <cstdint>
#include <string>
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../Level/Level.hpp"
#include "../Container/Inventory.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Client/GUI/Data.hpp"
#include "Texture2D.hpp"

class MinecraftUIRenderContext {
public:
    BUILD_ACCESS(this, ClientInstance*, clientInstance, GET_OFFSET("MinecraftUIRenderContext::clientInstance"));
    BUILD_ACCESS(this, class ScreenContext*, screenContext, GET_OFFSET("MinecraftUIRenderContext::screenContext"));
};