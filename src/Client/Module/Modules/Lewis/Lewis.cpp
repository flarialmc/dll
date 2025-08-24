#include "Lewis.hpp"

Lewis::Lewis(): Module("Lewis", "Lewis, king of Hippos. (Brought to you by the one and only, Oblitqrated) ",
                       IDR_FPS_PNG, "")
{
    

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
    setDef("lewisbounce", false);
    setDef("lewisbouncesize", 1.0f);
    setDef("lewisbouncespeed", 1.0f);
    setDef("lewiscrosshair", false);
    setDef("lewiscrosshairsize", 1.0f);
    setDef("lewislogo", false);
    setDef("lewisscream", false);
    
}

void Lewis::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Lewis");
    addToggle("Bouncing lewis", "", "lewisbounce");
    addSlider("Bouncing lewis size", "", "lewisbouncesize", 5);
    addSlider("Bouncing lewis speed", "", "lewisbouncespeed", 5);
    addToggle("Crosshair lewis", "", "lewiscrosshair");
    addSlider("Crosshair lewis size", "", "lewiscrosshairsize", 5);
    addToggle("Lewis Client Icon", "", "lewislogo");
    addToggle("Lewis Scream", "", "lewisscream");

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
