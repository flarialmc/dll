#include "CPSCounter.hpp"


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

bool CPSCounter::GetLeftHeld() {
    return leftClickHeld;
}

bool CPSCounter::GetRightHeld() {
    return rightClickHeld;
}

double CPSCounter::Microtime() {
    return (double(std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
}

void CPSCounter::onMouse(MouseEvent& event) {
    auto limiter = ModuleManager::getModule("CPS Limiter");
    if (limiter == nullptr) return;

    double now = getCurrentTime();

    if (event.getButton() == MouseButton::Left) {
        if (!MC::held) {
            leftClickHeld = false;
        }
        else {
            leftClickHeld = true;

            if (limiter->getOps<bool>("enabled")) {
                float leftCpsLimit = limiter->getOps<float>("Left");
                double leftInterval = 1.0 / leftCpsLimit;

                if ((now - lastLeftAllowed) < leftInterval) {
                    event.cancel();
                    return;
                }
                lastLeftAllowed = now;
            }
            AddLeftClick();
        }
    }

    if (event.getButton() == Right) {
        if (!MC::held) {
            rightClickHeld = false;
        }
        else {
            rightClickHeld = true;

            if (limiter->getOps<bool>("enabled")) {
                float rightCpsLimit = limiter->getOps<float>("Right");
                double rightInterval = 1.0 / rightCpsLimit;

                if ((now - lastRightAllowed) < rightInterval) {
                    event.cancel();
                    return;
                }
                lastRightAllowed = now;
            }
            AddRightClick();
        }
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