#include "BlockSource.hpp"


Block* BlockSource::getBlock(const Vec3<int>& bp) {
	return Memory::CallVFunc<2, Block*>(this, bp);
}

Biome* BlockSource::getBiome(const Vec3<int>& bp) {

    static uintptr_t sig;

    if(sig == NULL) sig = Memory::findSig("40 53 48 83 EC ? 48 8B D9 E8 ? ? ? ? 48 85 C0 75 5C");

    using efunc = Biome*(__cdecl*)(BlockSource*, const Vec3<int>&);
    auto func = reinterpret_cast<efunc>(sig);
    return func(this, bp);
}