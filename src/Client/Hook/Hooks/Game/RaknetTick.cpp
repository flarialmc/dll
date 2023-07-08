#include "RaknetTick.hpp"
#include "../../../../SDK/Client/Network/Raknet/RaknetConnector.hpp"
#include "../../../../SDK/SDK.hpp"


void RaknetTickHook::callback(RaknetConnector* raknet)  {

	SDK::raknetConnector = raknet;

}



void RaknetTickHook::enableHook() {
	this->autoHook(callback, (void**)raknetTickOriginal);
}
