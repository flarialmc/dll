#pragma once
#include "../Module.h"

class TestMod : public Module {
public:
	bool boolVar = false;
	float floatVar = 1.f;
public:
	TestMod() : Module("TestMod", "Test Mod for Developers", ICON_FA_ADDRESS_BOOK, 'K') {
		this->addBool("Test", "This is a test setting", &boolVar);
		this->addFloat("Test2", "This is a test setting", 1.f, 5.f, &floatVar);
	}
};