#include "LuaCommand.hpp"

#include <winrt/base.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>

#include <Utils/WinrtUtils.hpp>
#include <Scripting/ScriptManager.hpp>

#include <shlobj.h>
#include <string>

#include <miniz/miniz.h>
#include <Manager.hpp>

#include "embedded.h"

bool extractAutoComplete(const std::filesystem::path& output) {
    try {
        std::filesystem::path autoCompletePath = output / "AutoComplete";
        if (exists(autoCompletePath)) {
            remove_all(autoCompletePath);
        }

        mz_zip_archive zipArchive = {};
        if (!mz_zip_reader_init_mem(&zipArchive, autocomplete_zip, sizeof(autocomplete_zip), 0)) {
            Logger::error("Failed to initialize zip archive");
            return false;
        }

        int fileCount = static_cast<int>(mz_zip_reader_get_num_files(&zipArchive));
        for (int i = 0; i < fileCount; i++) {
            mz_zip_archive_file_stat fileStat;
            if (!mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
                continue;
            }

            std::filesystem::path filePath = output / fileStat.m_filename;

            if (mz_zip_reader_is_file_a_directory(&zipArchive, i)) {
                create_directories(filePath);
            } else {
                create_directories(filePath.parent_path());

                if (!mz_zip_reader_extract_to_file(&zipArchive, i, filePath.string().c_str(), 0)) {
                    Logger::error("Failed to extract the AutoComplete folder");
                }
            }
        }

        mz_zip_reader_end(&zipArchive);
        return true;
    }
    catch (const std::exception& e) {
        Logger::error("Error extracting AutoComplete: {}", e.what());
        return false;
    }
}

std::string scriptPath(const std::string& category) {
    const auto roamingFolder = to_string(winrt::Windows::Storage::ApplicationData::Current().RoamingFolder().Path());

    if (category == "module") {
        return (std::filesystem::path(roamingFolder) / "Flarial" / "Scripts" / "Modules").string();
    }
    if (category == "command") {
        return (std::filesystem::path(roamingFolder) / "Flarial" / "Scripts" / "Commands").string();
    }

    return "";
}

winrt::Windows::Foundation::IAsyncAction importScript(std::string category) {
    using namespace winrt::Windows::Storage;

    const auto selectedFiles = co_await WinrtUtils::pickFiles(L".lua");
    if (selectedFiles.Size() == 0) co_return;

    String::toLower(category);

    std::string targetDir = scriptPath(category);
    if (targetDir.empty()) co_return;

    std::error_code ec;
    std::filesystem::create_directories(targetDir, ec);
    if (ec) {
        Logger::error("Failed to create directory {}: {}", targetDir, ec.message());
        co_return;
    }

    for (uint32_t i = 0; i < selectedFiles.Size(); i++) {
        auto storageFile = selectedFiles.GetAt(i);
        auto fileName = to_string(storageFile.Name());
        std::filesystem::path destination = std::filesystem::path(targetDir) / fileName;

        try {
            co_await storageFile.CopyAsync(StorageFolder::GetFolderFromPathAsync(winrt::to_hstring(targetDir)).get(), winrt::to_hstring(fileName), NameCollisionOption::ReplaceExisting);
            Logger::success("Imported script: {}", fileName);
        } catch (const std::exception& e) {
            Logger::error("Failed to import script: {}", e.what());
        }
    }
}

void LuaCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        addCommandMessage("§cUsage: .lua <help/reload/path/import>");
        return;
    }

    std::string action = String::toLower(args[0]);

    if (action == "help") {
        addCommandMessage("§bpath §8- §7Opens the script directory in File Explorer");
        addCommandMessage("§breload §8- §7Reloads all scripts");
        addCommandMessage("§bimport §8- §7Automatically imports scripts for you");
        addCommandMessage("§bautocomplete §8- §7Extracts the latest AutoComplete docs");
    } else if (action == "path") {
        WinrtUtils::openSubFolder("Flarial\\Scripts");
    } else if (action == "reload") {
        ScriptManager::reloadScripts();
        addCommandMessage("§aReloaded scripts!");
    } else if (action == "import") {
        if (args.size() < 2) {
            addCommandMessage("§cUsage: .lua import <module/command>");
            return;
        }

        std::string category = String::toLower(args[1]);
        if (category != "module" && category != "command") {
            addCommandMessage("§cUsage: .lua import <module/command>");
            return;
        }

        [=]() -> winrt::fire_and_forget {
            co_await importScript(category);
        }();
    } else if (action == "autocomplete") {
        auto roamingFolder = to_string(winrt::Windows::Storage::ApplicationData::Current().RoamingFolder().Path());
        auto scriptsFolder = std::filesystem::path(roamingFolder) / "Flarial" / "Scripts";

        std::error_code ec;
        create_directories(scriptsFolder, ec);
        if (ec) {
            addCommandMessage("§cFailed to create Scripts folder: " + ec.message());
            return;
        }

        if (extractAutoComplete(scriptsFolder)) {
            addCommandMessage("§aImported the AutoComplete folder successfully.");
        } else {
            addCommandMessage("§cFailed to import the AutoComplete folder.");
        }
    } else {
        addCommandMessage("§cUsage: .lua <help/reload/path/import>");
    }
}