#pragma once

#include "../Texture2D.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class MaterialPtr {
private:
    char pad_0x0[0x138];

public:
    static MaterialPtr *createMaterial(HashedString const &materialName) {
        static void *materialCreator = nullptr;

        if (materialCreator == nullptr) {
            // Sig returns 6 addresses, all of them point to the same offset
            uintptr_t sigOffset = Memory::findSig(GET_SIG("Tessellator::createMaterial"));
            int offset = *reinterpret_cast<int *>(sigOffset + 3);
            materialCreator = reinterpret_cast<void *>(sigOffset + offset + 7);
        }

        return Memory::CallVFunc<1, MaterialPtr *>(materialCreator, materialName);
    }
};