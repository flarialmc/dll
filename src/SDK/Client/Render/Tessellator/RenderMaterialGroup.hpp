#pragma once

#include "../Material/MaterialPtr.hpp"

namespace mce {
    class RenderMaterialGroup {
    public:
        static MaterialPtr* createUI(const HashedString& materialName);
    };
}
