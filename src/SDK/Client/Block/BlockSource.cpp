#include "BlockSource.hpp"


Block* BlockSource::getBlock(const Vec3<int>& bp) {
	return Memory::CallVFunc<2, Block*>(this, bp);
}