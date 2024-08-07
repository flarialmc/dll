#pragma once

#include "../Level/Dimension.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../Level/Biome.hpp"
#include "../Level/Level.hpp"

class BlockSource {
public:
    mcDimension* getDimension();

    Biome *getBiome(const Vec3<int> &);
};