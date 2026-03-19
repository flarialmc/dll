#pragma once

#include "../HUDModule.hpp"
#include "../../../Client.hpp"

class IPDisplay : public HUDModule {

public:
	IPDisplay() : HUDModule(10, "IP Display",
		"Displays the current server IP you're playing on.",
		IDR_SERVER_IP_PNG, "", {"server", "address"}) {
	};

protected:
	std::string getDisplayValue() override;
	void customConfig() override;
	void customSettings() override;
};
