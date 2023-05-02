#pragma once
#include "../Module.h"

class Uninject : public Module {
public:
	Uninject() : Module("Uninject", "Remove the Client from the Game", ICON_FA_CODE, 'P') {};
};