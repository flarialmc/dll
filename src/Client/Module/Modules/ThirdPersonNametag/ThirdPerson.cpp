#include "ThirdPerson.hpp"

#include "Client.hpp"

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
    
}

void ThirdPerson::onEnable()
{
    patch();
    Listen(this, PerspectiveEvent, &ThirdPerson::onGetViewPerspective)
    Module::onEnable();
}

void ThirdPerson::onDisable()
{
    Deafen(this, PerspectiveEvent, &ThirdPerson::onGetViewPerspective)
    unpatch();
    Module::onDisable();
}

void ThirdPerson::patch()
{
    Memory::nopBytes((void *)address, 6);
    patched = true;
}

void ThirdPerson::unpatch()
{
    Memory::patchBytes((void *)address, original.data(), original.size());
    patched = false;
}

void ThirdPerson::onGetViewPerspective(PerspectiveEvent& event)
{
    if (event.getPerspective() == Perspective::FirstPerson and patched)  unpatch();
    else if(event.getPerspective() != Perspective::FirstPerson and !patched) patch();

}