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
    static void ApplyFadeOutEffect(float darkenModifier, float& darkenAmount, float leastAmount = 0.0)
    {
        if(darkenAmount < leastAmount) darkenAmount = leastAmount;
        else
        darkenAmount -= darkenModifier;
    }
};

