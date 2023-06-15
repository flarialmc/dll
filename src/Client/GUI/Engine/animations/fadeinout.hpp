#include <chrono>
#include <thread>

class FadeEffect
{
public:
    static void ApplyFadeInEffect(float darkenModifier, float maxDarkenAmount, float& darkenAmount)
    {
        darkenAmount += darkenModifier;
        if (darkenAmount > maxDarkenAmount)
            darkenAmount = maxDarkenAmount;
    }
    static void ApplyFadeOutEffect(float darkenModifier, float& darkenAmount)
    {
        darkenAmount -= darkenModifier;
        if(darkenAmount < 0.0) darkenAmount = 0.0;
    }
};

