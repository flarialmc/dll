#include "CPSCounter.hpp"

#include "Client.hpp"


void CPSCounter::onSetup() {
    Listen(this, MouseEvent, &CPSCounter::onMouse)
}

void CPSCounter::onEnable() {
    Listen(this, RenderEvent, &CPSCounter::onRender)
    Module::onEnable();
}

void CPSCounter::onDisable() {
    Deafen(this, RenderEvent, &CPSCounter::onRender)
    Module::onDisable();
}

void CPSCounter::defaultConfig() {
    setDef("text", (std::string)"CPS: {value}");
    setDef("rightcps", false);
    Module::defaultConfig("all");
    if (ModuleManager::initialized) Client::SaveSettings();
}

void CPSCounter::settingsRender(float settingsOffset) {

    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));


    addHeader("CPS Counter");
    defaultAddSettings("main");
    addToggle("Right Click CPS", "", "rightcps");
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

double CPSCounter::getCurrentTime() {
    using namespace std::chrono;
    return duration<double>(high_resolution_clock::now().time_since_epoch()).count();
}

void CPSCounter::AddLeftClick() {
    ClickData click{};
    click.timestamp = Microtime();
    leftClickList.insert(leftClickList.begin(), click);

    if (leftClickList.size() >= 100) {
        leftClickList.pop_back();
    }
}

void CPSCounter::AddRightClick() {
    ClickData click{};
    click.timestamp = Microtime();
    rightClickList.insert(rightClickList.begin(), click);

    if (rightClickList.size() >= 100) {
        rightClickList.pop_back();
    }
}

int CPSCounter::GetLeftCPS() {
    if (leftClickList.empty()) {
        return 0;
    }

    double currentMicros = Microtime();
    auto count = std::count_if(leftClickList.begin(), leftClickList.end(), [currentMicros](const ClickData& click) {
        return (currentMicros - click.timestamp <= 1.0);
    });

    return (int)std::round(count);
}

int CPSCounter::GetRightCPS() {
    if (rightClickList.empty()) {
        return 0;
    }

    double currentMicros = Microtime();
    auto count = std::count_if(rightClickList.begin(), rightClickList.end(),
                               [currentMicros](const ClickData& click) {
                                   return (currentMicros - click.timestamp <= 1.0);
                               });

    return (int)std::round(count);
}

double CPSCounter::Microtime() {
    return (double(std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
}

std::deque<std::chrono::steady_clock::time_point> leftClicks;
std::deque<std::chrono::steady_clock::time_point> rightClicks;

// Helper to try a click against a given deque + limit.
bool tryLimit(std::deque<std::chrono::steady_clock::time_point>& q, float cpsLimit) {
    using Clock = std::chrono::steady_clock;
    auto now = Clock::now();
    auto oneSecAgo = now - std::chrono::seconds(1);

    while (!q.empty() && q.front() < oneSecAgo) {
        q.pop_front();
    }

    if (static_cast<int>(q.size()) >= static_cast<int>(cpsLimit)) {
        return false;
    }

    q.push_back(now);
    return true;
}

void CPSCounter::onMouse(MouseEvent& event) {
    auto limiterMod = ModuleManager::getModule("CPS Limiter");
    if (!limiterMod)
        return;

    leftLimiter .setRate(limiterMod->getOps<float>("Left") + 3.f);
    rightLimiter.setRate(limiterMod->getOps<float>("Right") + 3.f);

    using MB = MouseButton;
    if (event.getButton() == MB::Left && MC::held) {
        if (!leftLimiter.allow() && limiterMod->getOps<bool>("enabled")) {
            //event.cancel();
            return;
        }
        AddLeftClick();
    }
    else if (event.getButton() == MB::Right && MC::held) {
        if (!rightLimiter.allow() && limiterMod->getOps<bool>("enabled")) {
            //event.cancel();
            return;
        }
        AddRightClick();
    }
}


void CPSCounter::onRender(RenderEvent& event)  {
    if (this->isEnabled()) {
        if (!getOps<bool>("rightcps")) {
            std::string leftCPS = FlarialGUI::cached_to_string(GetLeftCPS());
            this->normalRender(1, leftCPS);
        }
        else {
            std::string leftAndRightCPS = FlarialGUI::cached_to_string(GetLeftCPS()) + " | " + FlarialGUI::cached_to_string(GetRightCPS());
            this->normalRender(1, leftAndRightCPS);
        }

    }
}