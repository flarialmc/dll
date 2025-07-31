#pragma once

#include <string>
#include <memory>
#include <vector>
#include "JsonModel.hpp"
#include <stdexcept>
#include <filesystem>
#include "../../Config/Settings.hpp"

namespace cosmetic {
	struct JsonModel;

	class ModelLoaderException : public std::exception {
	private:
		std::string mMsg;
	public:
		explicit ModelLoaderException(std::string message) : mMsg(message) {}
		ModelLoaderException(ModelLoaderException const&) noexcept = default;

		const char* what() const noexcept override { return mMsg.c_str(); }
	};

	class ModelLoader {
	public:

		static std::vector<std::unique_ptr<JsonModel>> LoadFromFile(const std::filesystem::path& filepath);
		static std::vector<std::unique_ptr<JsonModel>> LoadFromStream(std::ifstream& in);
		static std::vector<std::unique_ptr<JsonModel>> LoadFromString(const std::string& str);
		static std::vector<std::unique_ptr<JsonModel>> LoadFromJson(const json& json);

		static std::unique_ptr<JsonModel> LoadFromFile(const std::filesystem::path& filepath, const std::string& geometryName);
		static std::unique_ptr<JsonModel> LoadFromStream(std::ifstream& in, const std::string& geometryName);
		static std::unique_ptr<JsonModel> LoadFromString(const std::string& str, const std::string& geometryName);
		static std::unique_ptr<JsonModel> LoadFromJson(const json& json, const std::string& geometryName);

	};
}