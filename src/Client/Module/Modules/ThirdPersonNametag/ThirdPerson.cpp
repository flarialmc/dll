#include "ThirdPerson.hpp"

ThirdPerson::ThirdPerson(): Module("Nametag", "Shows your nametag for you while\nin 3rd person mode.",
                                   IDR_NAMETAG_PNG, "")
{

    address = GET_SIG_ADDRESS("ThirdPersonNametag");

    original.resize(6);
    Memory::copyBytes((LPVOID) address, original.data(), 6);

    
}

void ThirdPerson::defaultConfig()
{
    Module::defaultConfig("core");
    Client::SaveSettings();
}

void ThirdPerson::onEnable()
{
    patch();
    Module::onEnable();
}

void ThirdPerson::onDisable()
{
    unpatch();
    Module::onDisable();
}

void ThirdPerson::patch()
{
    Memory::nopBytes((void *)address, 6);
}

void ThirdPerson::unpatch()
{
    Memory::patchBytes((void *)address, original.data(), original.size());
}
