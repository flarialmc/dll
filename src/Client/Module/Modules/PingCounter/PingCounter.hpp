#pragma once

#include "../HUDModule.hpp"

class PingCounter : public HUDModule {

public:

	PingCounter() : HUDModule(11, "Ping Counter", "Displays your current latency to the server.",
					IDR_PING_PNG, "", {"stats"}) {
	};

protected:
	std::string getDisplayValue() override;

	void customConfig() override;

	void customSettings() override;
};
