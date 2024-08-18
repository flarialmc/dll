#pragma once

#include <memory>

#include "RenderMaterialInfo.hpp"

namespace mce {
    class MaterialPtr {
    public:
        std::shared_ptr<RenderMaterialInfo> renderMaterialInfoPtr;
    };
}