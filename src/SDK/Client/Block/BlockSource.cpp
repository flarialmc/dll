#include "BlockSource.hpp"

mcDimension *BlockSource::getDimension() {
    return hat::member_at<mcDimension *>(this, GET_OFFSET("BlockSource::dimension"));
}

Biome *BlockSource::getBiome(const Vec3<int> &bp) {

    static uintptr_t sig;

    if (sig == NULL) sig = Memory::findSig(GET_SIG("BlockSource::getBiome"));

    using efunc = Biome *(__cdecl *)(BlockSource *, const Vec3<int> &);
    auto func = reinterpret_cast<efunc>(sig);
    return func(this, bp);
}