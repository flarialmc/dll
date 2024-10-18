#pragma once

#include <cstdint>

namespace mce {
    enum TextureFormat : uint32_t {
        UNKNOWN_TEXTURE_FORMAT = 0x0000,
        R32G32B32A32_FLOAT = 0x0002,
        R16G16B16A16_FLOAT = 0x000a,
        R16G16B16A16_UNORM = 0x000b,
        R32G32_FLOAT = 0x0010,
        R10G10B10A2_UNORM = 0x0018,
        R11G11B10_FLOAT = 0x001a,
        R8G8B8A8_UNORM = 0x001c,
        R8G8B8A8_UNORM_SRGB = 0x001d,
        R16G16_FLOAT = 0x0022,
        R16G16_UNORM = 0x0023,
        R16G16_UINT = 0x0024,
        R16G16_SNORM = 0x0025,
        D32_FLOAT = 0x0028,
        R32_FLOAT = 0x0029,
        R32_UINT = 0x002a,
        R24G8_TYPELESS = 0x002c,
        D24_UNORM_S8_UINT = 0x002d,
        R24_UNORM_X8_TYPELESS = 0x002e,
        R8G8_UNORM = 0x0031,
        R8G8_SNORM = 0x0033,
        R16_FLOAT = 0x0036,
        D16_UNORM = 0x0037,
        R8_UNORM = 0x003d,
        R8_UINT = 0x003e,
        A8_UNORM = 0x0041,
        BC3_UNORM = 0x004d,
        R5G6B5_UNORM = 0x0055,
        R5G5B5A1_UNORM = 0x0056,
        B8G8R8A8_UNORM = 0x0057,
        B8G8R8A8_UNORM_SRGB = 0x005b,
        BC7_UNORM = 0x0062,
        R4G4B4A4_UNORM = 0x0073,
        S8_UINT = 0x0074,
        R8G8B8_UNORM = 0x0075,
        COMPRESSED = 0x0076,
    };
}