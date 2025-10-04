#pragma once

#include <Command/Command.hpp>

struct Image;

class SkinStealCommand : public Command {
public:
    SkinStealCommand() : Command("steal", "Steal the skin of a player.") {};
    void execute(const std::vector<std::string>& args) override;
    static void SaveSkin(std::string name, Image image, Image capeImage);
    static std::vector<unsigned char> cropHead(const Image &originalImage, int type, bool head2 = false);
};