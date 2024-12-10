#pragma once

#include "RenderMaterialInfo.hpp"

#include <memory>

namespace mce {
    class MaterialPtr {
    public:
        std::shared_ptr<RenderMaterialInfo> renderMaterialInfoPtr;
    };
}