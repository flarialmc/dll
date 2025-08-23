#include "CosmeticManager.hpp"
#include "../Capes/CapeManager.hpp"


#include "Cosmetics/FlarialBandanna/FlarialBandanna.hpp"
#include "Cosmetics/Thug Life Glass/ThugLifeGlasses.hpp"
#include "Cosmetics/StarryNightBandanna/StarryNightBandanna.hpp"
#include "Cosmetics/Crown/Axolotl.hpp"

void CosmeticManager::Initialize()
{
    //addCosmetic<FlarialBandanna>();
    //addCosmetic<ThugLifeGlasses>();
    //addCosmetic<StarryNightBandanna>();
    addCosmetic<AxolotlHat>();
}
