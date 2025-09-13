#include "FixFontCommand.hpp"
#include "Client.hpp"

void FixFontCommand::execute(const std::vector<std::string> &args) {

	Client::settings.getSettingByName<bool>("overrideFontWeight")->value = false;
	Client::settings.getSettingByName<std::string>("fontWeight")->value = "Normal";
	Client::SavePrivate();
	Client::LoadPrivate();

	addCommandMessage("Successfully turned off \'Override Font Weight\' and reverted \'Font Weight\' to Normal");

}
