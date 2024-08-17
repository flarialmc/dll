#include "TextureGroup_getTextureHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/Render/GetTextureEvent.hpp"
#include "../../../Events/EventHandler.hpp"

TextureGroup_getTextureHook::TextureGroup_getTextureHook() : Hook("TextureGroup_getTexture", GET_SIG("mce::TextureGroup::getTexture")) {}

void TextureGroup_getTextureHook::enableHook() {
    this->autoHook((void *) TextureGroup_getTextureCallback, (void **) &funcOriginal);
}

TexturePtr* TextureGroup_getTextureHook::TextureGroup_getTextureCallback(TextureGroup *_this, TexturePtr *result,
                                                                  ResourceLocation *location, bool forceReload,
                                                                  void *a5, int a6, void *a7) {

    auto event = GetTextureEvent(location);
    EventHandler::onGetTexture(event);

    return funcOriginal(_this, result, event.location, forceReload, a5, a6, a7);
}