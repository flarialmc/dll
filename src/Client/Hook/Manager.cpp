#include "Manager.hpp"
#include "Hooks/Render/ResizeHook.hpp"
#include "Hooks/Render/CommandListHook.hpp"

std::vector<Hook *> HookManager::hooks;

void HookManager::initialize()
{
    MH_Initialize();

    hooks.push_back(new KeyHook());
    hooks.push_back(new MouseHook());
    hooks.push_back(new CommandListHook());

    hooks.push_back(new SwapchainHook());
    hooks.push_back(new ResizeHook());


    for (auto hook : hooks)
        hook->enableHook();

}

void HookManager::terminate()
{

    for (auto hook : hooks)
        delete hook;

    hooks.clear();
}