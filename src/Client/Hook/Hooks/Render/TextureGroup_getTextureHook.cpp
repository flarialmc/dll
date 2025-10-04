#include "TextureGroup_getTextureHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/Render/GetTextureEvent.hpp"

TextureGroup_getTextureHook::TextureGroup_getTextureHook() : Hook("TextureGroup_getTexture", GET_SIG_ADDRESS("mce::TextureGroup::getTexture")) {}

void TextureGroup_getTextureHook::enableHook() {
    this->autoHook((void *) TextureGroup_getTextureCallback, (void **) &funcOriginal);
}

TexturePtr* TextureGroup_getTextureHook::TextureGroup_getTextureCallback(TextureGroup *_this, TexturePtr *result,
                                                                  ResourceLocation *location, bool forceReload,
                                                                  void *a5, int a6, void *a7) {
    if(result == nullptr) return nullptr;

    auto retTexture = funcOriginal(_this, result, location, forceReload, a5, a6, a7);

    BedrockTextureData* bypassRefIncrement = *(BedrockTextureData**)&retTexture->clientTexture;

    auto event = nes::make_holder<GetTextureEvent>(location, bypassRefIncrement);
    eventMgr.trigger(event);

    return retTexture;
}