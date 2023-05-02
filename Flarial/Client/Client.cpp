#include "Client.h"
#include "Manager/Manager.h"

Client::Client(std::string n) {

	Logger::init();
	AllocConsole();
	SetConsoleTitleA("Flarial");
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout); // output only
	Logger::debug("Initializing Client");

	Manager::init();

	this->name = n.c_str();
	// this->mgr = new Manager(this);

};