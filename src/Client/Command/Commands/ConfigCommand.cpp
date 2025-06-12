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

#include "Modules/Misc/ScriptMarketplace/ScriptMarketplace.hpp"
namespace winrt
{
    using namespace Windows::Storage;
}
winrt::Windows::Foundation::IAsyncAction import() {


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
    }
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



