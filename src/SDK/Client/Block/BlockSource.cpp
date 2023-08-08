#include "BlockSource.hpp"


Block* BlockSource::getBlock(const Vec3<int>& bp) {
	return Memory::CallVFunc<2, Block*>(this, bp);
}

Biome* BlockSource::getBiome(const Vec3<int>& bp) {

    static uintptr_t sig;

    if(sig == NULL) sig = Memory::findSig("40 57 48 83 EC 50 48 8B F9 E8");

    using efunc = Biome*(__cdecl*)(BlockSource*, const Vec3<int>&);
    auto func = reinterpret_cast<efunc>(sig);
    return func(this, bp);
}