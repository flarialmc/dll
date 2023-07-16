#include "OnSuspend.hpp"
#include "../../../Module/Manager.hpp"



void OnSuspendHook::callback(void* a1, void* a2, void* a3, void* a4) {

	for (Module* mod : ModuleManager::modules) {
		mod->SaveSettings();
	}

	suspendOriginal(a1, a2, a3, a4);

}




void OnSuspendHook::enableHook() {

	this->autoHook(callback, (void**)&suspendOriginal);
}
