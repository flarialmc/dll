#include "ReachCounter.hpp"

#include "Events/EventManager.hpp"

ReachCounter::ReachCounter(): Module("Reach Counter", "Displays your last hit range in blocks.", IDR_REACH_PNG,
                                     "")
{
    
}

void ReachCounter::onEnable()
{
    Listen(this, AttackEvent, &ReachCounter::onAttack)
    Listen(this, TickEvent, &ReachCounter::onTick)
    Listen(this, RenderEvent, &ReachCounter::onRender)
    Module::onEnable();
}

void ReachCounter::onDisable()
{


    Module::onDisable();

}

void ReachCounter::defaultConfig()
{
    setDef("text", (std::string)"Reach: {value}");
    setDef("textscale", 0.70f);
    Module::defaultConfig("all");
    
}

void ReachCounter::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Reach Counter");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ReachCounter::onAttack(AttackEvent& event)
{
    if (!this->isEnabled()) return;
    Reach = event.getActor()->getLevel()->getHitResult().distance();
    last_hit = std::chrono::high_resolution_clock::now();
}

void ReachCounter::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
    if (duration.count() >= 15) Reach = 0.0f;

}

void ReachCounter::onRender(RenderEvent& event)
{
    if (this->isEnabled()) {

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << Reach;
        std::string reachString = oss.str();

        this->normalRender(9, reachString);

    }
}
