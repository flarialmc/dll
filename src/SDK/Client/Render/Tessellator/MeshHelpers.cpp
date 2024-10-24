#include "MeshHelpers.hpp"

#include "../../../../Utils/Versions/WinrtUtils.hpp"

void MeshHelpers::renderMeshImmediately(ScreenContext* screenContext, Tessellator* tessellator, mce::MaterialPtr* material) {
    if (WinrtUtils::checkAboveOrEqual(21, 20)) {
        char pad[0x58]{};
        static auto sig = Memory::offsetFromSig(GET_SIG_ADDRESS("MeshHelpers::renderMeshImmediately"), 1);
        using func_t = void(*)(ScreenContext*, Tessellator*, mce::MaterialPtr*, char*);
        static auto func = reinterpret_cast<func_t>(sig);
        func(screenContext, tessellator, material, pad);
    }
    else {
        static auto sig = GET_SIG_ADDRESS("MeshHelpers::renderMeshImmediately");
        using func_t = void(*)(ScreenContext*, Tessellator*, mce::MaterialPtr*);
        static auto func = reinterpret_cast<func_t>(sig);
        func(screenContext, tessellator, material);
    }
}
