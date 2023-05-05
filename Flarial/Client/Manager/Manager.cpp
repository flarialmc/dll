
#include "Manager.h"
#include "Hook/Hook.h"

/* Module Includes */

#include "Module/Modules/ClickGui.h"

#include "Module/Modules/AutoGG.h"
#include "Module/Modules/AutoSprint.h"
#include "Module/Modules/FpsCounter.h"

#include "Module/Modules/Uninject.h"
#include "Module/Modules/TestMod.h"

/* Hooks */

#include "Hook/Hooks/RenderContextHook.h"
#include "Hook/Hooks/SwapChainHook.h"
#include "Hook/Hooks/GameModeHook.h"
#include "Hook/Hooks/PacketSenderHook.h"
#include "Hook/Hooks/PacketHandlerHook.h"
#include "Hook/Hooks/EntityHook.h"
#include "Hook/Hooks/MouseHook.h"
#include "Hook/Hooks/KeyHook.h"

std::vector<Hook*> Manager::hooks;
std::vector<Module*> Manager::modules;

void Manager::init() {
	hooks.push_back(new RenderContextHook);
	hooks.push_back(new PacketSenderHook);
	hooks.push_back(new PacketHandlerHook);
	hooks.push_back(new GameModeHook);
	hooks.push_back(new EntityHook);
	hooks.push_back(new MouseHook);
	hooks.push_back(new KeyHook);
	hooks.push_back(new SwapChainHook);

	MH_EnableHook(MH_ALL_HOOKS);

	modules.push_back(new FpsCounter);
	modules.push_back(new AutoSprint);
	modules.push_back(new AutoGG);
	modules.push_back(new ClickGui);
	modules.push_back(new TestMod);
	modules.push_back(new Uninject);
}

void Manager::onKey(uintptr_t key, bool held, bool& cancel) {
	for (Module* mod : modules) {
		mod->onKey(key, held, cancel);
	}
}

void Manager::onMouse(MouseAction action, bool& cancel) {
	for (Module* mod : modules) {
		mod->onMouse(action, cancel);
	}
}

void Manager::onPacketSender(Packet* packet) {
	for (Module* mod : modules) {
		mod->onPacketSender(packet);
	}
}

void Manager::onPacketHandler(Packet* packet) {
	for (Module* mod : modules) {
		mod->onPacketHandler(packet);
	}
}

void Manager::onTick(GameMode* gm) {
	for (Module* mod : modules) {
		mod->onTick(gm);
	}
}

void Manager::onRender() {
	for (Module* mod : modules) {
		mod->onRender();
	}
}