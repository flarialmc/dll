#pragma once
#include "../Client.h"
#include "Hook/Hook.h"
#include "Module/Module.h"

enum class CategoryType;
class Category;
class Hook;
class Module;

class Manager {
public:
	static std::vector<Hook*> hooks;
	static std::vector<Module*> modules;
public:
	static void init();
public:
	static void onKey(uintptr_t key, bool held, bool& cancel);
	static void onMouse(MouseAction action, bool& cancel);
	static void onPacketSender(Packet* packet);
	static void onPacketHandler(Packet* packet);
	static void onTick(GameMode* gm);
	static void onRender();
};

class ImGuiNotif {
public:
	std::string title;
	std::string contents;
public:
	ImGuiToastType type;
public:
	ImGuiNotif(std::string t, std::string c, ImGuiToastType tp) {
		this->type = tp;
		this->title = t;
		this->contents = c;
	}
};