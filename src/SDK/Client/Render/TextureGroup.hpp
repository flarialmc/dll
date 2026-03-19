#pragma once

#include <map>
#include <optional>
#include <cstdint>
#include <atomic>
#include "ResourceLocation.hpp"
#include "BedrockTexture.hpp"
#include "TextureGroupBase.hpp"

#include "libhat/Access.hpp"
#include <SDK/Client/Bedrock/EnableNonOwnerReferences.hpp>

namespace mce {

    // ═══════════════════════════════════════════════════════════════════════════
    // mce::TextureGroup — Minecraft Bedrock's main texture management class.
    //
    // Inherits:
    //   Bedrock::EnableNonOwnerReferences  (safe non-owning pointer support)
    //   mce::TextureGroupBase              (abstract texture interface + enable_shared_from_this)
    //
    // Stored as shared_ptr<TextureGroup> in MinecraftGame and MinecraftUIRenderContext.
    //
    // ── Class Layout (IDA-verified for 1.21.13x, cross-referenced with LeviLamina) ──
    //
    // Offset  Field                              Size   Type
    // ──────  ─────                              ────   ────
    // +0x000  vftable (EnableNonOwnerReferences)    8   void**
    // +0x008  controlBlock                         16   shared_ptr<ControlBlock>
    // +0x018  vftable (TextureGroupBase)            8   void**  [TextureGroup::base]
    // +0x020  _Wptr (enable_shared_from_this)      16   weak_ptr<TextureGroupBase>
    // +0x030  mAdvancedGraphicsOptions             24   NotNullNonOwnerPtr<IAdvancedGraphicsOptions>
    // +0x048  mAsyncTextureLoader                 352   AsyncCachedTextureLoader
    // +0x1A8  mLoadedTextures                      16   std::map<ResourceLocation, BedrockTexture>
    // +0x1B8  mResourceLoadManager                 24   NotNullNonOwnerPtr<ResourceLoadManager>
    // +0x1D0  mResourceService                      8   TextureResourceService&
    // +0x1D8  mMissingTexture                      48   cg::ImageBuffer const
    // +0x208  mImageResourceLoader                 16   shared_ptr<ImageResourceLoader>
    // +0x218  mTextureSetResourceHelper             8   unique_ptr<TextureSetDefinitionLoader::ResourceHelper>
    // +0x220  mTouchedTextures                     16   std::map<ResourceLocation, std::optional<uint>>
    // +0x230  mImageBufferCache                    16   ImageBufferResourceManager
    // +0x240  mLRUCache                             8   unique_ptr<LRUCache>
    // +0x248  mLoadQueueResults                    16   std::map<pair<ResourceLocation,TextureLoadMode>, SharedAsync<void>>
    // +0x258  mQueueLoadResultCoroutine            16   Async<void>
    // +0x268  mPendingLoadTaskCounter              16   CountTracker
    // +0x278  mOutstandingTaskMemory                8   atomic<uint64>
    // +0x280  mOutstandingTaskCount                 8   atomic<uint64>
    //
    // Total size: ~0x288 bytes.
    //
    // Offsets are accessed through the SignatureAndOffsetManager system:
    //   - "TextureGroup::base"           = offset from TextureGroup* to TextureGroupBase subobject (0x18)
    //   - "TextureGroup::loadedTextures" = offset from TextureGroupBase* to mLoadedTextures
    //                                      (0x178 for <=1.21.11x, 0x190 for 1.21.13x+)
    // ═══════════════════════════════════════════════════════════════════════════

    class TextureGroup : public Bedrock::EnableNonOwnerReferences, public mce::TextureGroupBase {
    public:

        // ── Loaded Textures Map ──

        /// The primary texture cache — maps resource locations to loaded BedrockTexture entries.
        /// This is the map that getTexture() populates and that modules read from for texture swapping.
        std::map<ResourceLocation, BedrockTexture>& getLoadedTextures() {
            return hat::member_at<std::map<ResourceLocation, BedrockTexture>>(
                this, GET_OFFSET("TextureGroup::base") + GET_OFFSET("TextureGroup::loadedTextures"));
        }

        const std::map<ResourceLocation, BedrockTexture>& getLoadedTextures() const {
            return hat::member_at<const std::map<ResourceLocation, BedrockTexture>>(
                this, GET_OFFSET("TextureGroup::base") + GET_OFFSET("TextureGroup::loadedTextures"));
        }

        // ── Convenience Helpers ──
        // These work through getLoadedTextures() so they're automatically correct across versions.

        /// Check if a texture is present in the loaded cache.
        bool hasTexture(const ResourceLocation& location) const {
            return getLoadedTextures().contains(location);
        }

        /// Look up a loaded texture by location. Returns nullptr if not in the cache.
        BedrockTexture* findTexture(const ResourceLocation& location) {
            auto& textures = getLoadedTextures();
            auto it = textures.find(location);
            return it != textures.end() ? &it->second : nullptr;
        }

        const BedrockTexture* findTexture(const ResourceLocation& location) const {
            auto& textures = getLoadedTextures();
            auto it = textures.find(location);
            return it != textures.end() ? &it->second : nullptr;
        }

        /// Get the raw BedrockTextureData for a loaded texture, or nullptr.
        BedrockTextureData* getBedrockTextureData(const ResourceLocation& location) {
            auto* tex = findTexture(location);
            return (tex && tex->bedrockTextureData) ? tex->bedrockTextureData.get() : nullptr;
        }

        /// Check if a texture is fully loaded (not just pending or missing).
        bool isTextureLoaded(const ResourceLocation& location) const {
            auto* tex = findTexture(location);
            if (!tex || !tex->bedrockTextureData) return false;
            return (tex->bedrockTextureData->textureLoadState & TextureLoadState::LoadedBit) != 0;
        }

        /// Get the number of loaded textures in the cache.
        size_t getLoadedTextureCount() const {
            return getLoadedTextures().size();
        }
    };

} // namespace mce

// Backward-compatible alias: existing code uses unqualified `TextureGroup` everywhere
// (MinecraftGame.hpp, hooks, modules). This keeps all of that working without modification.
using TextureGroup = mce::TextureGroup;
