#pragma once

#include <cstdint>

namespace mce {
    enum BindFlagsBit : uint32_t {
        NoBindFlags = 0x0000,
        VertexBufferBit = 0x0001,
        IndexBufferBit = 0x0002,
        ConstantBufferBit = 0x0004,
        ShaderResourceBit = 0x0008,
        StreamOutputBit = 0x0010,
        RenderTargetBit = 0x0020,
        DepthStencilBit = 0x0040,
        UnorderedAccessBit = 0x0080,
    };
}