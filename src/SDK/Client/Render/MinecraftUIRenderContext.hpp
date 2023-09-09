#pragma once


#include "../Core/ClientInstance.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "ScreenContext.hpp"
#include "textHolder.hpp"
#include <cstdint>
#include <string>
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../Level/Level.hpp"
#include "../Container/Inventory.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Client/GUI/D2D.hpp"
#include "TextMeasureData.hpp"
#include "Texture2D.hpp"

class MinecraftUIRenderContext {
public:
	BUILD_ACCESS(this, ClientInstance*, clientInstance, 0x8);
	BUILD_ACCESS(this, ScreenContext*, screenContext, 0x10);
};