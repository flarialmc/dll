#pragma once


#include "../Core/ClientInstance.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include <cstdint>
#include <string>
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../Level/Level.hpp"
#include "../Container/Inventory.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Client/GUI/D2D.hpp"
#include "TexturePtr.hpp"
#include "TextureGroup.hpp"
#include "../../../Client/Module/Manager.hpp"

class MinecraftUIRenderContext {
public:
    ClientInstance* getClientInstance() {
        return hat::member_at<ClientInstance*>(this, GET_OFFSET("MinecraftUIRenderContext::clientInstance"));
    }

    class ScreenContext* getScreenContext() {
        return hat::member_at<ScreenContext*>(this, GET_OFFSET("MinecraftUIRenderContext::screenContext"));
    }

    std::shared_ptr<TextureGroup>& getTextureGroup() {
        return hat::member_at<std::shared_ptr<TextureGroup>>(this, GET_OFFSET("MinecraftUIRenderContext::textures"));
    }

    std::map<ResourceLocation, BedrockTexture>& getTextures() {
        return getTextureGroup()->getLoadedTextures();
    }

    TexturePtr* getTexture(TexturePtr* ptr, const ResourceLocation& location, bool forceReload) {
        using getTextureFunc = TexturePtr*(__fastcall*)(MinecraftUIRenderContext*, TexturePtr*, const ResourceLocation&, bool);
        static auto getTextureOriginal = reinterpret_cast<getTextureFunc>(Memory::findSig(GET_SIG("MinecraftUIRenderContext::getTexture")));
        return getTextureOriginal(this, ptr, location, forceReload);
    }

    TexturePtr* createTexture(const std::string& path, bool external, bool forceReload) {
        const auto resource = ResourceLocation(path, external);
        TexturePtr texture = TexturePtr();
        return getTexture(&texture, resource, forceReload);
    }

    TexturePtr* createTexture(const ResourceLocation& location, bool forceReload) {
        TexturePtr texture = TexturePtr();
        return getTexture(&texture, location, forceReload);
    }

    void reloadTexture(const ResourceLocation& location) {
        this->createTexture(location, true);
    }

    void reloadAllTextures() {
        auto& textures = this->getTextures();

        for (auto& [resourceLocation, texture]  : textures) {
            texture.unload();
            reloadTexture(resourceLocation);
        }
    }

    void swapTextures(ResourceLocation& from, ResourceLocation& to) {
        auto& textures = this->getTextures();

        // only reload if texture is not loaded
        if(!textures.contains(from)) {
            reloadTexture(from);
        }

        if(!textures.contains(to)) {
            reloadTexture(to);
        }

        if(textures.contains(from) && textures.contains(to) && textures[from] != textures[to]) {
            if(textures[to].bedrockTextureData->textureLoadState == TextureLoadState::LoadedBit) {
                textures[from] = textures[to];
            } else {
                auto texture = createTexture(to, false);
                textures[from] = *texture->clientTexture;
            }
        }
    }
};