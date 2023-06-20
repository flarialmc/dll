#include <chrono>
#include <thread>

class FadeEffect
{
public:
    static void ApplyFadeInEffect(float darkenModifier, float maxDarkenAmount, float& darkenAmount)
    {

        if (darkenAmount < maxDarkenAmount) darkenAmount += darkenModifier;
    }
    static void ApplyFadeOutEffect(float darkenModifier, float& darkenAmount, float leastAmount = 0.0)
    {
        if(darkenAmount - darkenModifier > leastAmount) darkenAmount -= darkenModifier;
    }
};

