#include "CosmeticManager.hpp"
#include "../Capes/CapeManager.hpp"


#include "Cosmetics/FlarialBandanna/FlarialBandanna.hpp"
#include "Cosmetics/Thug Life Glass/ThugLifeGlasses.hpp"
#include "Cosmetics/StarryNightBandanna/StarryNightBandanna.hpp"
#include "Cosmetics/Crown/Axolotl.hpp"
#include "Cosmetics/HeartsAura/HeartsAura.hpp"

void CosmeticManager::Initialize()
{
    //addCosmetic<FlarialBandanna>();
    //addCosmetic<ThugLifeGlasses>();
    //addCosmetic<StarryNightBandanna>();
    //addCosmetic<AxolotlHat>();
    addCosmetic<HeartAura>();
}
