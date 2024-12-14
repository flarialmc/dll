#include "EjectCommand.hpp"

#include <Client/Client.hpp>

void EjectCommand::execute(const std::vector<std::string> &args) {
    Client::disable = true;
}
