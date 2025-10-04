#pragma once

#include <SDK/Client/Core/ClientInstance.hpp>
#include <SDK/Client/Core/HashedString.hpp>
#include <Client/Module/Manager.hpp>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Utils.hpp>

#include "TexturePtr.hpp"
#include "TextureGroup.hpp"
#include "ScreenContext.hpp"

#include <cstdint>
#include <string>

class MinecraftUIRenderContext {
public:
    ClientInstance* getClientInstance() {
        return hat::member_at<ClientInstance*>(this, GET_OFFSET("MinecraftUIRenderContext::clientInstance"));
    }

    ScreenContext* getScreenContext() {
        return hat::member_at<ScreenContext*>(this, GET_OFFSET("MinecraftUIRenderContext::screenContext"));
    }

    std::shared_ptr<TextureGroup>& getTextureGroup() {
        return hat::member_at<std::shared_ptr<TextureGroup>>(this, GET_OFFSET("MinecraftUIRenderContext::textures"));
    }

    std::map<ResourceLocation, BedrockTexture>& getTextures() {
        return getTextureGroup()->getLoadedTextures();
    }

    TexturePtr& getTexture(TexturePtr& ptr, const ResourceLocation& location, bool forceReload) {
        return Memory::CallVFuncI<TexturePtr&>(GET_OFFSET("MinecraftUIRenderContext::getTexture"), this, ptr, location, forceReload);
    }

    TexturePtr& createTexture(const std::string& path, bool external, bool forceReload) {
        const auto resource = ResourceLocation(path, external);
        TexturePtr texture;
        return getTexture(texture, resource, forceReload);
    }

    TexturePtr& createTexture(const ResourceLocation& location, bool forceReload) {
        TexturePtr texture;
        return getTexture(texture, location, forceReload);
    }

    void drawImage(TexturePtr& texture, Vec2<float>& pos, Vec2<float>& size, Vec2<float>& uvPos, Vec2<float>& uvSize) {
        if (VersionUtils::checkAboveOrEqual(21, 50))
            Memory::CallVFunc<7, void, BedrockTextureData*, Vec2<float>&, Vec2<float>&, Vec2<float>&, Vec2<float>&, bool>(this, texture.clientTexture.get(), pos, size, uvPos, uvSize, false);
        else if (VersionUtils::checkAboveOrEqual(21, 20))
            Memory::CallVFunc<7, void, TexturePtr&, Vec2<float>&, Vec2<float>&, Vec2<float>&, Vec2<float>&, bool>(this, texture, pos, size, uvPos, uvSize, false);
        else
            Memory::CallVFunc<7, void, TexturePtr&, Vec2<float>&, Vec2<float>&, Vec2<float>&, Vec2<float>&>(this, texture, pos, size, uvPos, uvSize);
    }

    void flushImages(mce::Color &color, float opacity, HashedString &hashedString) {
        Memory::CallVFunc<9, void, mce::Color&, float, HashedString &>(this, color, opacity, hashedString);
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
                textures[from] = *texture.clientTexture;
            }
        }
    }
};