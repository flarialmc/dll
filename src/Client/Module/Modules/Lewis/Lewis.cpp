#include "Lewis.hpp"

Lewis::Lewis(): Module("Lewis", "Lewis, king of Hippos. (Brought to you by the one and only, Oblitqrated) ",
                       IDR_FPS_PNG, "")
{
    Module::setup();

    HRSRC imageResHandle = nullptr;
    HGLOBAL imageResDataHandle = nullptr;
    const unsigned char* pImageFile = nullptr;
    DWORD imageFileSize = 0;


    imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(253), "MP3");
    if (imageResHandle == nullptr) {
        return;
    }

    imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
    if (imageResDataHandle == nullptr) {
        return;
    }

    pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
    if (pImageFile == nullptr) {
        return;
    }

    imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
    if (imageFileSize == 0) {
        return;
    }

    saveMp3File(pImageFile, imageFileSize, Utils::getAssetsPath() + "\\lewis.mp3");
}

void Lewis::onEnable()
{
    Listen(this, RenderEvent, &Lewis::onRender)
    Module::onEnable();
}

void Lewis::onDisable()
{
    Deafen(this, RenderEvent, &Lewis::onRender)
    Module::onDisable();
}

void Lewis::defaultConfig()
{
    Module::defaultConfig("core");
    if (settings.getSettingByName<bool>("lewisbounce") == nullptr) settings.addSetting("lewisbounce", false);
    if (settings.getSettingByName<float>("lewisbouncesize") == nullptr) settings.addSetting("lewisbouncesize", 1.0f);
    if (settings.getSettingByName<float>("lewisbouncespeed") == nullptr) settings.addSetting("lewisbouncespeed", 1.0f);
    if (settings.getSettingByName<bool>("lewiscrosshair") == nullptr) settings.addSetting("lewiscrosshair", false);
    if (settings.getSettingByName<float>("lewiscrosshairsize") == nullptr) settings.addSetting("lewiscrosshairsize", 1.0f);
    if (settings.getSettingByName<bool>("lewislogo") == nullptr) settings.addSetting("lewislogo", false);
    if (settings.getSettingByName<bool>("lewisscream") == nullptr) settings.addSetting("lewisscream", false);
}

void Lewis::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Lewis");
    addToggle("Bouncing lewis", "", settings.getSettingByName<bool>("lewisbounce")->value);
    addSlider("Bouncing lewis size", "", settings.getSettingByName<float>("lewisbouncesize")->value, 5);
    addSlider("Bouncing lewis speed", "", settings.getSettingByName<float>("lewisbouncespeed")->value, 5);
    addToggle("Crosshair lewis", "", settings.getSettingByName<bool>("lewiscrosshair")->value);
    addSlider("Crosshair lewis size", "", settings.getSettingByName<float>("lewiscrosshairsize")->value, 5);
    addToggle("Lewis Client Icon", "", settings.getSettingByName<bool>("lewislogo")->value);
    addToggle("Lewis Scream", "", settings.getSettingByName<bool>("lewisscream")->value);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Lewis::onRender(RenderEvent& event)
{
    if (this->isEnabled()) {
        if (this->settings.getSettingByName<bool>("lewisbounce")->value) {
            LPCTSTR mode = "PNG";
            float s = Constraints::RelativeConstraint(0.35, "height", true) * this->settings.getSettingByName<float>("lewisbouncesize")->value;
            FlarialGUI::image(IDR_LEWIS_PNG, D2D1::RectF(x, y, x + s, y + s), mode);

            x += this->settings.getSettingByName<float>("lewisbouncespeed")->value * xv;
            y += this->settings.getSettingByName<float>("lewisbouncespeed")->value * yv;

            if (x >= MC::windowSize.x - s) xv = -1;
            if (x < 0) xv = 1;
            if (y >= MC::windowSize.y - s) yv = -1;
            if (y < 0) yv = 1;
        }

        if (this->settings.getSettingByName<bool>("lewiscrosshair")->value)
        {
            float s = Constraints::RelativeConstraint(0.25, "height", true) * this->settings.getSettingByName<float>("lewiscrosshairsize")->value;
            Vec2<float> pos((MC::windowSize.x / 2) - (s / 2), (MC::windowSize.y / 2) - (s / 2));
            FlarialGUI::image(IDR_LEWIS_PNG, D2D1::RectF(pos.x, pos.y, pos.x + s, pos.y + s), "PNG");
        }

        if (this->settings.getSettingByName<bool>("lewisscream")->value)
        {
            if ((NextLewisScream - std::chrono::system_clock::now()).count() < 0)
            {
                std::thread thread([&]()
                {
                    Audio::play(Utils::getAssetsPath() + "\\lewis.mp3");
                });

                thread.detach();

                std::random_device rd;  // Seed
                std::mt19937 gen(rd()); // Mersenne Twister engine
                std::uniform_int_distribution<> distrib(30, 300); // Range [30, 300]

                // Generate and output random number
                int randomNum = distrib(gen);

                NextLewisScream = std::chrono::system_clock::now() + std::chrono::seconds(randomNum);
            }
        }
    }
}
