#include "BlockSource.hpp"


Block* BlockSource::getBlock(const Vec3<int>& bp) {
	return Memory::CallVFunc<2, Block*>(this, bp);
}

Biome* BlockSource::getBiome(const Vec3<int>& bp) {
    return Memory::CallVFunc<28, Biome*>(this, bp);
}