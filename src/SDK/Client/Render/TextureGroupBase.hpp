#pragma once

#include <memory>
#include <optional>
#include <cstdint>
#include "TexturePtr.hpp"
#include "Textures/TextureSetLayerType.hpp"

namespace mce {

    // TextureGroupBase is the abstract interface for texture management.
    // It inherits from enable_shared_from_this because the game stores TextureGroup
    // in shared_ptrs (e.g. MinecraftGame::textureGroup) and needs weak self-references.
    //
    // Layout (from LeviLamina + IDA verification):
    //   +0x00  vtable ptr
    //   +0x08  weak_ptr from enable_shared_from_this (16 bytes)
    //   +0x18  start of derived class data
    class TextureGroupBase : public std::enable_shared_from_this<TextureGroupBase> {
    public:
        virtual ~TextureGroupBase() = default;

        // Full signature confirmed by IDA and LeviLamina:
        //   RCX = this
        //   RDX = hidden return ptr (TexturePtr, 32 bytes — returned via hidden pointer on MSVC x64)
        //   R8  = ResourceLocation const&
        //   R9  = bool forceReload
        //   [RSP+0x28] = std::optional<uint32_t> optLoadOrder
        //   [RSP+0x30] = cg::TextureSetLayerType textureType
        //
        // Previous Flarial declaration only had the first two params; the remaining
        // stack slots were passed as uninitialized garbage from call sites. Adding
        // defaults (nullopt, Color) makes calls correct without changing existing code.
        virtual TexturePtr getTexture(
            const ResourceLocation& location,
            bool forceReload,
            std::optional<uint32_t> optLoadOrder = std::nullopt,
            cg::TextureSetLayerType textureType = cg::TextureSetLayerType::Color
        ) = 0;
    };

} // namespace mce
