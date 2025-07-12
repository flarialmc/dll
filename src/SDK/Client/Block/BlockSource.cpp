#include "BlockSource.hpp"

Block *BlockSource::getBlock(const BlockPos & pos) {
    return Memory::CallVFuncI<Block *>(GET_OFFSET("BlockSource::getBlock"), this, pos);
}

Dimension *BlockSource::getDimension() {
    return hat::member_at<Dimension *>(this, GET_OFFSET("BlockSource::dimension"));
}

Biome *BlockSource::getBiome(const BlockPos &bp) {

    static uintptr_t sig;

    if (sig == NULL) sig = GET_SIG_ADDRESS("BlockSource::getBiome");

    using efunc = Biome *(__cdecl *)(BlockSource *, const BlockPos &);
    auto func = reinterpret_cast<efunc>(sig);
    return func(this, bp);
}