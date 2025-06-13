#include <fstream>
#include "getTimeOfDayHook.hpp"
#include "../../../Client.hpp"

void getTimeOfDayHook::enableHook() {
	this->autoHook(callback, (void**)&funcOriginal);
}

getTimeOfDayHook::getTimeOfDayHook() : Hook("getTimeOfDayHook", GET_SIG_ADDRESS("LevelUtils::getTimeOfDay")) {}

float getTimeOfDayHook::callback(__int64 a1, int a2, float a3) {

	float something = funcOriginal(a1, a2, a3);

	Logger::debug("{}", something);

	return something;
}