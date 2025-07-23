#include "CosmeticManager.hpp"


#include "Cosmetics/FlarialBandanna/FlarialBandanna.hpp"
#include "Cosmetics/Thug Life Glass/ThugLifeGlasses.hpp"

void CosmeticManager::Initialize()
{
    //addCosmetic<FlarialBandanna>();
    addCosmetic<ThugLifeGlasses>();
}
