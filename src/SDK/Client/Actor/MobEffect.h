#pragma once

#include <memory>

class MobEffect {
public:
	static std::unique_ptr<MobEffect, std::default_delete<MobEffect>>* getEffects();
};