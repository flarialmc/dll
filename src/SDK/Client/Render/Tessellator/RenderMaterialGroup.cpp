#include "RenderMaterialGroup.hpp"

#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

mce::MaterialPtr* mce::RenderMaterialGroup::createUI(const HashedString& materialName) {
    static auto uiRenderMaterialGroup = Memory::getOffsetFromSig<void*>(Memory::findSig(GET_SIG("mce::RenderMaterialGroup::ui")), 3);

    return Memory::CallVFunc<1, MaterialPtr*, const HashedString&>(uiRenderMaterialGroup, materialName);
}
