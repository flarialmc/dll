#pragma once


#include "../Core/ClientInstance.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "ScreenContext.hpp"
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

	MaterialPtr* getUIMaterial() {
		static uintptr_t ref;

		if (ref == 0) {

			//The MaterialPtr is called "ui_fill_color, it has a static pointer to its instance.
		
			//ScreenRenderer::fillRectangle
			ref = Memory::findSig("4C 8D 05 ? ? ? ? 48 8B D3 48 8B CF 48 8B 5C 24 ? 0F 28 7C 24 ? 44 0F 28 44 24 ? 48 83 C4 40 5F E9 ? ? ? ?");
		}

		int re = *reinterpret_cast<int*>(ref + 3);
		return reinterpret_cast<MaterialPtr*>(ref + re + 7);
	}
};