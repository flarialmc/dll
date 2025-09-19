#include "FixFontCommand.hpp"
#include "Client.hpp"

void FixFontCommand::execute(const std::vector<std::string> &args) {

	Client::settings.getSettingByName<bool>("overrideFontWeight")->value = false;
	Client::settings.getSettingByName<std::string>("fontWeight")->value = "Normal";
	Client::settings.getSettingByName<float>("gui_font_scale")->value = 1.f;
	Client::settings.getSettingByName<float>("modules_font_scale")->value = 1.f;
	Client::SavePrivate();
	Client::LoadPrivate();

	addCommandMessage("Successfully turned off \'Override Font Weight\' and reset font weight and scales.");

}
