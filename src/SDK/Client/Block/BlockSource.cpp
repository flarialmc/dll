#include "BlockSource.hpp"

Dimension *BlockSource::getDimension() {
    return hat::member_at<Dimension *>(this, GET_OFFSET("BlockSource::dimension"));
}

Biome *BlockSource::getBiome(const Vec3<int> &bp) {

    static uintptr_t sig;

    if (sig == NULL) sig = GET_SIG_ADDRESS("BlockSource::getBiome");

    using efunc = Biome *(__cdecl *)(BlockSource *, const Vec3<int> &);
    auto func = reinterpret_cast<efunc>(sig);
    return func(this, bp);
}