#pragma once

#include <SDK/Client/Render/Material/MaterialPtr.hpp>

namespace mce {
    class RenderMaterialGroup {
    public:
        static MaterialPtr* createUI(const HashedString& materialName);
    };
}
