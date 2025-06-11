#include "Coordinates.hpp"

#include "Events/EventManager.hpp"

void Coordinates::onEnable() {
    Listen(this, RenderEvent, &Coordinates::onRender)
    Module::onEnable();
}

void Coordinates::onDisable() {
    Deafen(this, RenderEvent, &Coordinates::onRender)
    Module::onDisable();
}

void Coordinates::defaultConfig() {
    setDef("responsivewidth", true);
    setDef("text", (std::string) "{D} X: {X} Y: {Y} Z: {Z}");
    setDef("textscale", 0.80f);
    Module::defaultConfig("all");
    setDef("showDecimals", false);
    setDef("decimalCount", 2.f);
    setDef("verticalMode", false);
    setDef("showVelocity", true);
    setDef("showYvelocity", true);
    setDef("showDimName", false);
    setDef("useSameDimensionFormat", true);
    setDef("defaultDimFormat", (std::string) "{dim}");
    setDef("OverworldFormat", (std::string) "{dim}");
    setDef("NetherFormat", (std::string) "{dim}");
    setDef("TheEndFormat", (std::string) "{dim}");
    setDef("showOtherDimCoords", false);
    if (ModuleManager::initialized) Client::SaveSettings();
}

void Coordinates::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Coordinates");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    addToggle("Show Decimals", "", "showDecimals");
    addConditionalSlider(getOps<bool>("showDecimals"), "Number of Decimals", "", getOps<float>("decimalCount"), 6.f,
                         1.f);
    extraPadding();

    addHeader("Module Settings");
    addToggle("Vertical Mode", "", "verticalMode");
    addConditionalToggle(getOps<bool>("verticalMode"), "Show Velocity", "+ or -", "showVelocity");
    addConditionalToggle(getOps<bool>("verticalMode") && getOps<bool>("showVelocity"), "Show Y-axis Velocity", "", "showYvelocity");
    addConditionalToggle(!getOps<bool>("verticalMode"), "Show Dimension Name", "Shows the dimension name.", "showDimName");
    addConditionalToggle(!getOps<bool>("verticalMode"), "Use Same Dimension Format", "Uses the same format for all dimensions.", "useSameDimensionFormat");
    addConditionalTextBox(!getOps<bool>("verticalMode") && getOps<bool>("useSameDimensionFormat"), "Dimension Format", "",
                          getOps<std::string>("defaultDimFormat"));
    addConditionalTextBox(!getOps<bool>("verticalMode") && !getOps<bool>("useSameDimensionFormat"), "Overworld Dimension Format", "",
                          getOps<std::string>("OverworldFormat"));
    addConditionalTextBox(!getOps<bool>("verticalMode") && !getOps<bool>("useSameDimensionFormat"), "Nether Dimension Format", "",
                          getOps<std::string>("NetherFormat"));
    addConditionalTextBox(!getOps<bool>("verticalMode") && !getOps<bool>("useSameDimensionFormat"), "TheEnd Dimension Format", "",
                          getOps<std::string>("TheEndFormat"));
    addConditionalToggle(!getOps<bool>("verticalMode") && !getOps<bool>("verticalMode"), "Show Other Dimension Coords",
              "Shows the other dimension\'s coordinates along with overworld coordinates.", "showOtherDimCoords");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

StringMap Coordinates::getCoords(float multiplier) {
    Vec3<float> *pos = SDK::clientInstance->getLocalPlayer()->getPosition();

    int decimalsToShow = getOps<bool>("showDecimals") ? std::floor(getOps<float>("decimalCount")) : -1;

    std::string xstr = std::to_string(pos->x * multiplier);
    std::string ystr = std::to_string(pos->y * multiplier);
    std::string zstr = std::to_string(pos->z * multiplier);

    xstr.erase(xstr.size() - (6 - decimalsToShow));
    ystr.erase(ystr.size() - (6 - decimalsToShow));
    zstr.erase(zstr.size() - (6 - decimalsToShow));

    return StringMap{
        {"x", xstr},
        {"y", ystr},
        {"z", zstr}
    };
}

std::string Coordinates::formattedDimension(std::string dimension) {
    if (getOps<bool>("useSameDimensionFormat")) return std::regex_replace(getOps<std::string>("defaultDimFormat"), std::regex("\\{dim\\}"), dimension);
    else return std::regex_replace(getOps<std::string>(dimension + "Format"), std::regex("\\{dim\\}"), dimension);
}

std::string Coordinates::sigma(float val) {
    if (val < 0) return "-";
    else if (val > 0) return "+";
    else return " ";
}

void Coordinates::onRender(RenderEvent &event) {
    if (this->isEnabled() && SDK::clientInstance->getLocalPlayer() && SDK::getCurrentScreen() == "hud_screen") {

        if (!getOps<bool>("verticalMode")) {
            std::string coord = getOps<std::string>("text");
            std::string dimension = SDK::clientInstance->getBlockSource()->getDimension()->getName();

            if (dimension == "TheEnd") {
                StringMap pos = getCoords(1.0f);
                coord = std::regex_replace(coord, std::regex("\\{X\\}"), pos["x"]);
                coord = std::regex_replace(coord, std::regex("\\{Y\\}"), pos["y"]);
                coord = std::regex_replace(coord, std::regex("\\{Z\\}"), pos["z"]);

                if (getOps<bool>("showDimName")) {
                    coord = std::regex_replace(coord, std::regex("\\{D\\}"), formattedDimension("TheEnd"));
                } else {
                    coord = std::regex_replace(coord, std::regex("\\{D\\}"), "");
                }

                this->normalRenderCore(33, coord);
            } else {
                std::string coord2 = coord;

                float otherDimMultiplier = 0.0f;
                std::string otherDim = "";
                if (dimension == "Overworld") {
                    otherDimMultiplier = 0.125f;
                    otherDim = "Nether";
                } else if (dimension == "Nether") {
                    otherDimMultiplier = 8.0f;
                    otherDim = "Overworld";
                }

                StringMap defaultPos = getCoords(1.0f);

                coord = std::regex_replace(coord, std::regex("\\{X\\}"), defaultPos["x"]);
                coord = std::regex_replace(coord, std::regex("\\{Y\\}"), defaultPos["y"]);
                coord = std::regex_replace(coord, std::regex("\\{Z\\}"), defaultPos["z"]);

                if (getOps<bool>("showDimName")) {
                    coord = std::regex_replace(coord, std::regex("\\{D\\}"), formattedDimension(dimension));
                } else {
                    coord = std::regex_replace(coord, std::regex("\\{D\\}"), "");
                }

                std::string text = coord;

                if (getOps<bool>("showOtherDimCoords")) {
                    StringMap otherDimPos = getCoords(otherDimMultiplier);
                    coord2 = std::regex_replace(coord2, std::regex("\\{X\\}"), otherDimPos["x"]);
                    coord2 = std::regex_replace(coord2, std::regex("\\{Y\\}"), otherDimPos["y"]);
                    coord2 = std::regex_replace(coord2, std::regex("\\{Z\\}"), otherDimPos["z"]);

                    if (getOps<bool>("showDimName")) {
                        coord2 = std::regex_replace(coord2, std::regex("\\{D\\}"), formattedDimension(otherDim));
                    } else {
                        coord2 = std::regex_replace(coord2, std::regex("\\{D\\}"), "");
                    }

                    text = coord + "\n" + coord2;
                }

                this->normalRenderCore(33, text);
            }
        }
        else {
            StringMap pos = getCoords(1.f);

            std::string text;

            if (getOps<bool>("showVelocity")) {
                Vec3<float> velo = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent()->velocity;
                text += std::format("{}X {}\n", sigma(velo.x), pos["x"]);
                if (getOps<bool>("showYvelocity")) text += std::format("{}Y {}\n", sigma(velo.y), pos["y"]);
                else text += std::format(" Y {}\n", pos["y"]);
                text += std::format("{}Z {}", sigma(velo.z), pos["z"]);
            }
            else {
                text += std::format("X {}\nY {}\nZ {}", pos["x"], pos["y"], pos["z"]);
            }

            this->normalRenderCore(33, text);
        }
    }
}
