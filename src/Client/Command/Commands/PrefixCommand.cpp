#include "PrefixCommand.hpp"
#include "../../Client.hpp"

void PrefixCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        addCommandMessage("§cUsage: .prefix <key>");
        return;
    }

    if (args[0].length() != 1) {
        addCommandMessage("§cYour prefix must be 1 character long.");
        return;
    }

    Client::settings.setValue("dotcmdprefix", std::string(args[0]));
    addCommandMessage("§aYour prefix has been set to §b{}", Client::settings.getSettingByName<std::string>("dotcmdprefix")->value);
    Client::SavePrivate();
    Client::LoadPrivate();
}
