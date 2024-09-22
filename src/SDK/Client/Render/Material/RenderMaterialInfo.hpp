#pragma once

#include "../../Core/HashedString.hpp"

namespace mce {
    class RenderMaterialInfo : std::enable_shared_from_this<RenderMaterialInfo> {
    public:
        HashedString name;
        // std::unique_ptr<mce::RenderMaterial> ptr;
    };
}
