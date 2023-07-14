#include "OnSuspend.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Client.hpp"


void OnSuspendHook::callback(void* a1, void* a2, void* a3, void* a4) {

	for (Module* mod : ModuleManager::modules) {
		mod->SaveSettings();
	}

    Client::SaveSettings();

	suspendOriginal(a1, a2, a3, a4);

}




void OnSuspendHook::enableHook() {

	this->autoHook(callback, (void**)&suspendOriginal);
}
