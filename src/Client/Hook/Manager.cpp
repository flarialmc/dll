#include "Manager.hpp"

void HookManager::initialize()
{
    MH_Initialize();
    hooks = std::vector<Hook *>();

    hooks.push_back(new KeyHook());

    for (auto hook : hooks)
        hook->enableHook();
}

void HookManager::terminate()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    for (auto hook : hooks)
        delete hook;

    hooks.clear();
}