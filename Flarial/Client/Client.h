#pragma once
#include "SDK/Minecraft.h"
#include "Config/Config.h"

class Manager;

class Client {
public:
	std::string name;
public:
	Manager* mgr = nullptr;
public:
	Client(std::string);
};