#pragma once
#include <cstdint>
#include <memory>

namespace mce {

    enum class ImageFormat : int {
        Unknown = 0,
        R8Unorm = 1,
        RG8Unorm = 2,
        RGB8Unorm = 3,
        RGBA8Unorm = 4,
        RGBA16Float = 5,
    };

    enum class ImageUsage : char {
        Unknown = 0,
        SRGB = 1,
        Data = 2,
    };

    class Blob {
        using Deleter = void(*)(uint8_t*);
        std::unique_ptr<uint8_t[], Deleter> data;
        size_t size;
    };

	class Image {
	public:
        ImageFormat imageFormat;
        int Width;
        int Height;
        int Depth;
        ImageUsage Usage;
        Blob mImageBytes;
	};
}