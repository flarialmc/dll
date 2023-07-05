#include "ClientInstance.hpp"
#include "../../SDK.hpp"

std::string ClientInstance::getTopScreenName() {
    return SDK::CurrentScreen;
}