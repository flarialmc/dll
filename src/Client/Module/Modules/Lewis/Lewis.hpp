#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"
#include "Utils/Audio.hpp"
#include <random>

#include "Client.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"


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

	Lewis() : Module("Lewis", "Lewis, king of Hippos. (Brought to you by the one and only, Oblitqrated) ",
					   IDR_SKULL_PNG, "", false, {"troll"}) {

		HRSRC imageResHandle = nullptr;
		HGLOBAL imageResDataHandle = nullptr;
		const unsigned char* pImageFile = nullptr;
		DWORD imageFileSize = 0;


		imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(253), "MP3");
		if (imageResHandle == nullptr) {
			return;
		}

		imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
		if (imageResDataHandle == nullptr) {
			return;
		}

		pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
		if (pImageFile == nullptr) {
			return;
		}

		imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
		if (imageFileSize == 0) {
			return;
		}

		saveMp3File(pImageFile, imageFileSize, Utils::getAssetsPath() + "\\lewis.mp3");
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};
