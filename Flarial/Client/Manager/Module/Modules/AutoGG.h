#include "../Module.h"

namespace Triggers {
	static std::string hive = "Game OVER!";
}

class AutoGG : public Module {
public:
	AutoGG() : Module("Auto GG", "Automatically say gg after game end", ICON_FA_TEXT, 'G') {};

	virtual void onPacketHandler(Packet* packet) override;
};