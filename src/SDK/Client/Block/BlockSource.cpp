#include "BlockSource.hpp"

Biome *BlockSource::getBiome(const Vec3<int> &bp) {

    static uintptr_t sig;

    if (sig == NULL) sig = Memory::findSig(GET_SIG("BlockSource::getBiome"));

    using efunc = Biome *(__cdecl *)(BlockSource *, const Vec3<int> &);
    auto func = reinterpret_cast<efunc>(sig);
    return func(this, bp);
}