#include "Client.h"
#include "Manager/Manager.h"

Client::Client(std::string n) {

	Logger::init();
	Logger::debug("Initializing Client");

	Manager::init();

	this->name = n.c_str();
	// this->mgr = new Manager(this);

};