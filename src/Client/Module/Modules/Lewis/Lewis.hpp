#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"
#include "Utils/Audio.hpp"
#include <random>

#include "Client.hpp"
#include "Events/EventManager.hpp"

inline void saveMp3File(const unsigned char* byteArray, size_t size, const std::string& outputPath) {
	// Open the file in binary mode for writing
	std::ofstream outFile(outputPath, std::ios::binary);

	// Check if the file was opened successfully
	if (!outFile) {
		throw std::runtime_error("Failed to open file for writing: " + outputPath);
	}

	// Write the byte array to the file
	outFile.write(reinterpret_cast<const char*>(byteArray), size);

	// Check for write errors
	if (!outFile) {
		throw std::runtime_error("Error occurred while writing to file: " + outputPath);
	}

	// Close the file
	outFile.close();
}

class Lewis : public Module {
	float x = 0;
	float y = 0;
	float xv = 1;
	float yv = 1;

	std::chrono::time_point<std::chrono::system_clock> NextLewisScream = std::chrono::system_clock::now();

public:

	Lewis();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};
