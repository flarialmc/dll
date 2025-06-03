#include <fstream>
#include "getTimeOfDayHook.hpp"
#include "../../../Client.hpp"

void getTimeOfDayHook::enableHook() {
    this->autoHook(callback, (void**)&funcOriginal);
}

getTimeOfDayHook::getTimeOfDayHook() : Hook("getTimeOfDayHook", GET_SIG_ADDRESS("LevelUtils::getTimeOfDay")) {}

int getTimeOfDayHook::callback(void* a1) {
    int something = funcOriginal(a1);

    Logger::debug("{}", something);

    return something;
}