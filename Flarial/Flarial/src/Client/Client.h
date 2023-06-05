#pragma once
#include "Managers/Backend/Manager.h"
#include "Managers/Frontend/Manager.h"

class Client
{
public:
    static BackendManager* BackendManager;
    static FrontendManager* FrontendManager;
public:
    static void initialize();
    
};
