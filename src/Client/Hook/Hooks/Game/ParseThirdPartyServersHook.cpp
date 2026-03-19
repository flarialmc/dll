#include "ParseThirdPartyServersHook.hpp"

bool ParseThirdPartyServersHook::callback(void* a1,
    Json::Value* resultsArray,
    int count,
    void* backend,
    int32_t searchType) {


    Logger::debug("{}", resultsArray->toStyledString());
    
    return original(a1, resultsArray, count, backend, searchType);
}

void ParseThirdPartyServersHook::enableHook() {
    this->autoHook((void*)callback, (void**)&original);
}
