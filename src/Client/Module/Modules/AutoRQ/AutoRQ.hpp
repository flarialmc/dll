#pragma once

#include "../Module.hpp"
#include "../Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"


class AutoRQ : public Module {
private:
	bool triggered = false;
	int deaths = 0;
	std::string teamcolor = "THIS IS NEEDED TRUST!!!!!!!!!!!!!!";
	std::string Copy_CS = "THIS IS NEEDED EVEN MORE TRUST!!!!";
public:
	AutoRQ() : Module("Hive Utils", "Handy utilities for The Hive partnered server",
		IDR_HIVE_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;


	void onPacketReceive(PacketEvent& event);

	void reQ();

	void onKey(KeyEvent& event);
};
