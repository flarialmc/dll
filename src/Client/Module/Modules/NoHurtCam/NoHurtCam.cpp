#include "NoHurtCam.hpp"

#include "Events/EventManager.hpp"

NoHurtCam::NoHurtCam(): Module("No Hurt Cam", "Disables hurt camera animation", IDR_REACH_PNG, "")
{
    int size;
    if (VersionUtils::checkAboveOrEqual(21, 30)) {
        size = 5;
    } else {
        size = 3;
    }

    originalCameraAngle.resize(size);

    if(sigOffset == NULL) {
        if (VersionUtils::checkAboveOrEqual(21, 30)) {
            sigOffset = GET_SIG_ADDRESS("CameraAssignAngle");
        } else {
            sigOffset = GET_SIG_ADDRESS("CameraAssignAngle") + 4;
        }
    }

    Memory::patchBytes( originalCameraAngle.data(), (LPVOID)sigOffset, size);
    
}

void NoHurtCam::onEnable()
{
    Listen(this, RaknetTickEvent, &NoHurtCam::onRaknetTick)
    Listen(this, TickEvent, &NoHurtCam::onTick)
    Module::onEnable();
}

void NoHurtCam::onDisable()
{
    Deafen(this, RaknetTickEvent, &NoHurtCam::onRaknetTick)
    Deafen(this, TickEvent, &NoHurtCam::onTick)
    Module::onDisable();
}

void NoHurtCam::defaultConfig()
{
    Module::defaultConfig("core");
    if (ModuleManager::initialized) Client::SaveSettings();
}

void NoHurtCam::patch()
{
    if(patched) return;
    patched = true;
    int size;
    if (VersionUtils::checkAboveOrEqual(21, 70)) {
        size = 1;
    } else if (VersionUtils::checkAboveOrEqual(21, 30)) {
        size = 5;
    } else {
        size = 3;
    }
    if (VersionUtils::checkAboveOrEqual(21, 70)) {
        std::byte jump{0xEB};
        Memory::patchBytes((LPVOID) sigOffset, &jump, size);
    } else {
        Memory::nopBytes((LPVOID) sigOffset, size);
    }
}

void NoHurtCam::unpatch()
{
    if(!patched) return;
    patched = false;
    int size;
    if (VersionUtils::checkAboveOrEqual(21, 70)) {
        size = 1;
    } else if (VersionUtils::checkAboveOrEqual(21, 30)) {
        size = 5;
    } else {
        size = 3;
    }
    Memory::patchBytes((LPVOID) sigOffset, originalCameraAngle.data(), size);
}

void NoHurtCam::onRaknetTick(RaknetTickEvent& event)
{
    if (this->isEnabled()) {
        std::string serverIP = SDK::getServerIP();
        if (serverIP.find("hive") != std::string::npos) {
            if (!this->restricted) {
                FlarialGUI::Notify("Can't use No Hurt Cam on " + serverIP); // TODO: move restrictions to API
                this->restricted = true;
            }
        } else {
            this->restricted = false;
        }
    }
}

void NoHurtCam::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (!this->restricted) {
        patch();
    } else {
        unpatch();
    }
}
