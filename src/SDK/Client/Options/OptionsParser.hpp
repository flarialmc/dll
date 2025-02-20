#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "../../Utils/Logger/Logger.hpp"

class OptionsParser
{
public:
	std::map<std::string, std::string> options;
	 std::map<std::string, std::string> parseOptionsFile();
};