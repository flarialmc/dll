#include "ConfigCommand.hpp"

#include <winrt/base.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>

#include <Utils/WinrtUtils.hpp>
#include <Scripting/ScriptManager.hpp>

#include <shlobj.h>
#include <string>
#include <winrt/Windows.Storage.Streams.h>
#include <miniz/miniz.h>
#include <Manager.hpp>
#include <random>
#include <filesystem>
#include <fstream>

#include "Modules/Misc/ScriptMarketplace/ScriptMarketplace.hpp"
#include <GUI/D2D.hpp>
#include <GUI/Engine/EngineImGui.hpp>
namespace winrt
{
    using namespace Windows::Storage;
}
winrt::Windows::Foundation::IAsyncAction importZip() {
    const auto selectedFiles = co_await WinrtUtils::pickFiles(L".zip");
    if (selectedFiles.Size() == 0) co_return;
    for (uint32_t i = 0; i < selectedFiles.Size(); i++)
    {
        auto zipFile = selectedFiles.GetAt(i);
        auto zipFileName = to_string(zipFile.Name());
        std::filesystem::path zipPath(zipFileName);
        std::string folderName = zipPath.stem().string();
        std::filesystem::path extractionDir = std::filesystem::path(Utils::getConfigsPath()) / "Legacy" / folderName;
        std::error_code ec;
        std::filesystem::create_directories(extractionDir, ec);
        if (ec)
        {
            LOG_ERROR("Failed to create extraction directory {}: {}", extractionDir.string(), ec.message());
            continue;
        }
        std::wstring zipFilePathW = zipFile.Path().c_str();
        std::string zipFilePath = winrt::to_string(zipFilePathW);
        mz_zip_archive zip_archive;
        memset(&zip_archive, 0, sizeof(zip_archive));
        auto stream = co_await zipFile.OpenAsync(winrt::FileAccessMode::Read);
        auto buffer = co_await winrt::FileIO::ReadBufferAsync(zipFile);
        std::vector<uint8_t> data(buffer.Length());
        winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer).ReadBytes(data);

        if (!mz_zip_reader_init_mem(&zip_archive, data.data(), data.size(), 0))
        {
            LOG_ERROR("Failed to initialize zip archive from memory: {}", zipFileName);
            continue;
        }
        int fileCount = static_cast<int>(mz_zip_reader_get_num_files(&zip_archive));
        for (int j = 0; j < fileCount; j++)
        {
            mz_zip_archive_file_stat file_stat;
            if (!mz_zip_reader_file_stat(&zip_archive, j, &file_stat))
            {
                LOG_ERROR("Failed to get file stat for file index {} in zip {}", j, zipFileName);
                continue;
            }
            std::filesystem::path destPath = extractionDir / file_stat.m_filename;
            if (mz_zip_reader_is_file_a_directory(&zip_archive, j))
            {
                std::filesystem::create_directories(destPath, ec);
                if (ec)
                {
                    LOG_ERROR("Failed to create directory {}: {}", destPath.string(), ec.message());
                }
            }
            else
            {
                std::filesystem::create_directories(destPath.parent_path(), ec);
                if (ec)
                {
                    LOG_ERROR("Failed to create directory {}: {}", destPath.parent_path().string(), ec.message());
                    continue;
                }
                if (!mz_zip_reader_extract_to_file(&zip_archive, j, destPath.string().c_str(), 0))
                {
                    LOG_ERROR("Failed to extract file {} in zip {}", file_stat.m_filename, zipFileName);
                }
            }
        }
        mz_zip_reader_end(&zip_archive);
        Logger::success("Extracted zip file: {} to {}", zipFileName, extractionDir.string());
        FlarialGUI::Notify("Created & loaded: " + zipFileName);
        ModuleManager::restartModules = true;
        ScriptMarketplace::reloadAllConfigs();
    }
    co_return;
}

winrt::Windows::Foundation::IAsyncAction importJson() {
    const auto selectedFiles = co_await WinrtUtils::pickFiles(L".json");
    if (selectedFiles.Size() == 0) co_return;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    
    for (uint32_t i = 0; i < selectedFiles.Size(); i++)
    {
        auto jsonFile = selectedFiles.GetAt(i);
        
        // Generate random number for config name
        int randomNum = dis(gen);
        std::string configName = "imported-" + std::to_string(randomNum);
        
        // Ensure the config name is unique
        std::filesystem::path configPath = std::filesystem::path(Utils::getConfigsPath()) / (configName + ".json");
        while (std::filesystem::exists(configPath)) {
            randomNum = dis(gen);
            configName = "imported-" + std::to_string(randomNum);
            configPath = std::filesystem::path(Utils::getConfigsPath()) / (configName + ".json");
        }
        
        try {
            // Read the JSON file content
            auto buffer = co_await winrt::FileIO::ReadBufferAsync(jsonFile);
            std::vector<uint8_t> data(buffer.Length());
            winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer).ReadBytes(data);
            
            // Convert to string
            std::string jsonContent(data.begin(), data.end());
            
            // Write to new config file
            std::ofstream outFile(configPath);
            if (outFile.is_open()) {
                outFile << jsonContent;
                outFile.close();
                
                Logger::success("Imported JSON config: {} as {}", to_string(jsonFile.Name()), configName);
                FlarialGUI::Notify("Imported config: " + configName);
            } else {
                LOG_ERROR("Failed to write config file: {}", configPath.string());
                FlarialGUI::Notify("Failed to import: " + to_string(jsonFile.Name()));
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to import JSON config {}: {}", to_string(jsonFile.Name()), e.what());
            FlarialGUI::Notify("Failed to import: " + to_string(jsonFile.Name()));
        }
    }
    
    // Reload configs to include the new ones
    ScriptMarketplace::reloadAllConfigs();
    co_return;
}

winrt::Windows::Foundation::IAsyncAction import() {
    // Show a picker that allows all files, then filter by extension
    const auto selectedFiles = co_await WinrtUtils::pickFiles(L"*");
    if (selectedFiles.Size() == 0) co_return;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    
    for (uint32_t i = 0; i < selectedFiles.Size(); i++)
    {
        auto file = selectedFiles.GetAt(i);
        auto fileName = to_string(file.Name());
        std::filesystem::path filePath(fileName);
        std::string extension = String::toLower(filePath.extension().string());
        
        if (extension == ".zip") {
            // Handle legacy .zip import
            std::string folderName = filePath.stem().string();
            std::filesystem::path extractionDir = std::filesystem::path(Utils::getConfigsPath()) / "Legacy" / folderName;
            std::error_code ec;
            std::filesystem::create_directories(extractionDir, ec);
            if (ec)
            {
                LOG_ERROR("Failed to create extraction directory {}: {}", extractionDir.string(), ec.message());
                continue;
            }
            
            try {
                mz_zip_archive zip_archive;
                memset(&zip_archive, 0, sizeof(zip_archive));
                auto buffer = co_await winrt::FileIO::ReadBufferAsync(file);
                std::vector<uint8_t> data(buffer.Length());
                winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer).ReadBytes(data);

                if (!mz_zip_reader_init_mem(&zip_archive, data.data(), data.size(), 0))
                {
                    LOG_ERROR("Failed to initialize zip archive from memory: {}", fileName);
                    continue;
                }
                int fileCount = static_cast<int>(mz_zip_reader_get_num_files(&zip_archive));
                for (int j = 0; j < fileCount; j++)
                {
                    mz_zip_archive_file_stat file_stat;
                    if (!mz_zip_reader_file_stat(&zip_archive, j, &file_stat))
                    {
                        LOG_ERROR("Failed to get file stat for file index {} in zip {}", j, fileName);
                        continue;
                    }
                    std::filesystem::path destPath = extractionDir / file_stat.m_filename;
                    if (mz_zip_reader_is_file_a_directory(&zip_archive, j))
                    {
                        std::filesystem::create_directories(destPath, ec);
                        if (ec)
                        {
                            LOG_ERROR("Failed to create directory {}: {}", destPath.string(), ec.message());
                        }
                    }
                    else
                    {
                        std::filesystem::create_directories(destPath.parent_path(), ec);
                        if (ec)
                        {
                            LOG_ERROR("Failed to create directory {}: {}", destPath.parent_path().string(), ec.message());
                            continue;
                        }
                        if (!mz_zip_reader_extract_to_file(&zip_archive, j, destPath.string().c_str(), 0))
                        {
                            LOG_ERROR("Failed to extract file {} in zip {}", file_stat.m_filename, fileName);
                        }
                    }
                }
                mz_zip_reader_end(&zip_archive);
                Logger::success("Extracted zip file: {} to {}", fileName, extractionDir.string());
                FlarialGUI::Notify("Created & loaded: " + fileName);
                ModuleManager::restartModules = true;
            } catch (const std::exception& e) {
                LOG_ERROR("Failed to import ZIP config {}: {}", fileName, e.what());
                FlarialGUI::Notify("Failed to import: " + fileName);
            }
        }
        else if (extension == ".json") {
            // Handle new .json import
            int randomNum = dis(gen);
            std::string configName = "imported-" + std::to_string(randomNum);
            
            // Ensure the config name is unique
            std::filesystem::path configPath = std::filesystem::path(Utils::getConfigsPath()) / (configName + ".json");
            while (std::filesystem::exists(configPath)) {
                randomNum = dis(gen);
                configName = "imported-" + std::to_string(randomNum);
                configPath = std::filesystem::path(Utils::getConfigsPath()) / (configName + ".json");
            }
            
            try {
                // Read the JSON file content
                auto buffer = co_await winrt::FileIO::ReadBufferAsync(file);
                std::vector<uint8_t> data(buffer.Length());
                winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer).ReadBytes(data);
                
                // Convert to string
                std::string jsonContent(data.begin(), data.end());
                
                // Write to new config file
                std::ofstream outFile(configPath);
                if (outFile.is_open()) {
                    outFile << jsonContent;
                    outFile.close();
                    
                    Logger::success("Imported JSON config: {} as {}", fileName, configName);
                    FlarialGUI::Notify("Imported config: " + configName);
                } else {
                    LOG_ERROR("Failed to write config file: {}", configPath.string());
                    FlarialGUI::Notify("Failed to import: " + fileName);
                }
            } catch (const std::exception& e) {
                LOG_ERROR("Failed to import JSON config {}: {}", fileName, e.what());
                FlarialGUI::Notify("Failed to import: " + fileName);
            }
        }
        else {
            // Unsupported file type
            FlarialGUI::Notify("Unsupported file type: " + fileName + " (only .zip and .json supported)");
        }
    }
    
    // Reload configs to include any new ones
    ScriptMarketplace::reloadAllConfigs();
    co_return;
}

void ConfigCommand::execute(const std::vector<std::string>& args) {
    if (!args.empty()) {
        std::string action = String::toLower(args[0]);

        if (action == "reload") ScriptMarketplace::reloadAllConfigs();
        else if (action == "create" && args.size() == 2) { Client::createConfig(args[1]); FlarialGUI::Notify("Created " + args[1]); }
        else if (action == "delete" && args.size() == 2) { Client::deleteConfig(args[1]); FlarialGUI::Notify("Deleted " + args[1]); }
        else if (action == "select" && args.size() == 2) { if (std::filesystem::exists(Utils::getRoamingPath() + "\\Flarial\\Config\\" + args[1])) {Client::settings.getSettingByName<std::string>("currentConfig")->value = args[1]; FlarialGUI::Notify("Selected " + args[1]);} }
        else if (action == "import") {
            [=]() -> winrt::fire_and_forget {
                co_await import();
            }();
        }
    } else {
        addCommandMessage("§cUsage: .config <create/delete/reload/import>");
    }
}



