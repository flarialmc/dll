#include <chrono>
#include <thread>

class FadeEffect
{
public:
    static void ApplyFadeInEffect(float darkenModifier, float maxDarkenAmount, float& darkenAmount)
    {
        if (Client::settings.getSettingByName<bool>("disableanims")->value) darkenAmount = maxDarkenAmount;
        else if (darkenAmount + darkenModifier < maxDarkenAmount) darkenAmount += darkenModifier;
    }
    static void ApplyFadeOutEffect(float darkenModifier, float& darkenAmount, float leastAmount = 0.0)
    {
        if (Client::settings.getSettingByName<bool>("disableanims")->value) darkenAmount = leastAmount;
        else if (darkenAmount - darkenModifier > leastAmount) darkenAmount -= darkenModifier;
    }
};

