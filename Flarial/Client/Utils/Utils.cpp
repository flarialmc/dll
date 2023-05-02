#include "Utils.h"

std::string Utils::getRoamPath() {

	char* path = NULL;
	size_t length;

	_dupenv_s(&path, &length, "appdata");

	if (path == NULL)
		return "";

	return std::string(path) + "\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState";

};

std::string Utils::sanitize(std::string text) { // https://github.com/horionclient/Horion/blob/cb636b579c27388c59c44705549263a4f9407573/Utils/Utils.cpp#L34
	std::string out;
	bool wasValid = true;

	auto invalidChar = [](char c) {
		return !(c >= 0 && *reinterpret_cast<unsigned char*>(&c) < 128);
	};

	for (char c : text) {
		bool isValid = !invalidChar(c);
		if (wasValid) {
			if (!isValid) {
				wasValid = false;
			}
			else {
				out += c;
			}
		}
		else {
			wasValid = isValid;
		}
	};

	return out;
};

std::string Utils::toLower(std::string input) {

	std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) {

		return std::tolower(c);

				   });

	return input;

};

auto Utils::splitStringBy(std::string in, std::string splitBy) -> std::vector<std::string> {

	auto out = std::vector<std::string>();

	std::stringstream ss(in);
	std::string segment;

	while (std::getline(ss, segment, *splitBy.c_str())) {

		out.push_back(segment);

	};

	return out;

};