#pragma once
#include "Managers/Hook/Manager.h"
#include "Managers/Frontend/Manager.h"
#include "Managers/Module/Manager.h"

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
