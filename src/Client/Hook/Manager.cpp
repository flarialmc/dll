#include "Manager.hpp"

std::vector<Hook *> HookManager::hooks;

void HookManager::initialize()
{
    MH_Initialize();

    hooks.push_back(new KeyHook());
    hooks.push_back(new MouseHook());

    for (auto hook : hooks)
        hook->enableHook();

}

void HookManager::terminate()
{

    for (auto hook : hooks)
        delete hook;

    hooks.clear();
}