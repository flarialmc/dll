#pragma once
#include "Managers/Hook/Manager.hpp"
#include "Managers/Frontend/Manager.hpp"
#include "Managers/Module/Manager.hpp"

class Client
{
public:
    HooksManager* HooksManager;
    FrontendManager* FrontendManager;
    ModulesManager* ModulesManager;
    
    void initialize();

    class HooksManager getHooksManager() const;
    class FrontendManager getFrontendManager() const;
    class ModulesManager getModulesManager() const;
};
