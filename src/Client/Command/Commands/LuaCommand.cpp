#include "LuaCommand.hpp"

#include <winrt/base.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Web.Http.h>

#include <Utils/WinrtUtils.hpp>
#include <Scripting/ScriptManager.hpp>

#include <string>

#include <miniz/miniz.h>
#include <Manager.hpp>

winrt::Windows::Foundation::IAsyncOperation<bool> extractAutoComplete() {
    auto roamingFolder = winrt::Windows::Storage::ApplicationData::Current().RoamingFolder();
    auto autoCompleteFolder = std::filesystem::path(to_string(roamingFolder.Path())) / "Flarial" / "Scripts" / "AutoComplete";

    try {
        winrt::Windows::Web::Http::HttpClient httpClient;
        auto response = co_await httpClient.GetAsync(winrt::Windows::Foundation::Uri(L"https://github.com/flarialmc/scripting-wiki/archive/refs/heads/main.zip"));
        if (response.StatusCode() != winrt::Windows::Web::Http::HttpStatusCode::Ok) {
            Logger::error("Failed to download repository zip from GitHub");
            co_return false;
        }

        auto tempFile = co_await roamingFolder.CreateFileAsync(L"repo.zip", winrt::Windows::Storage::CreationCollisionOption::ReplaceExisting);
        auto stream = co_await tempFile.OpenAsync(winrt::Windows::Storage::FileAccessMode::ReadWrite);
        co_await response.Content().WriteToStreamAsync(stream);
        stream.Close();

        std::string tempFilePath = to_string(tempFile.Path());
        std::string prefix = "scripting-wiki-main/autocomplete/";

        if (exists(autoCompleteFolder)) {
            remove_all(autoCompleteFolder);
        }
        create_directories(autoCompleteFolder);

        mz_zip_archive zipArchive = {};
        if (!mz_zip_reader_init_file(&zipArchive, tempFilePath.c_str(), 0)) {
            Logger::error("Failed to initialize zip archive from downloaded file");
            co_await tempFile.DeleteAsync();
            co_return false;
        }

        int fileCount = static_cast<int>(mz_zip_reader_get_num_files(&zipArchive));
        for (int i = 0; i < fileCount; i++) {
            mz_zip_archive_file_stat fileStat;
            if (!mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
                continue;
            }

            std::string fileName = fileStat.m_filename;
            if (fileName.starts_with(prefix)) {
                std::string relativePath = fileName.substr(prefix.length());
                std::filesystem::path localPath = autoCompleteFolder / relativePath;

                // If it's a directory, create it.
                if (mz_zip_reader_is_file_a_directory(&zipArchive, i)) {
                    create_directories(localPath);
                } else {
                    // Create the parent directory.
                    create_directories(localPath.parent_path());
                    if (!mz_zip_reader_extract_to_file(&zipArchive, i, localPath.string().c_str(), 0)) {
                        Logger::error("Failed to extract file: {}", fileName);
                    }
                }
            }
        }

        mz_zip_reader_end(&zipArchive);
        co_await tempFile.DeleteAsync();
        co_return true;
    } catch (const std::exception& e) {
        Logger::error("Error extracting AutoComplete: {}", e.what());
        co_return false;
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
        addCommandMessage("§cUsage: .lua <help/reload/path/import/autocomplete>");
        return;
    }

    std::string action = String::toLower(args[0]);

    if (action == "help") {
        addCommandMessage("§bpath §8- §7Opens the script directory in File Explorer");
        addCommandMessage("§breload §8- §7Reloads all scripts");
        addCommandMessage("§bimport §8- §7Automatically imports scripts for you");
        addCommandMessage("§bautocomplete §8- §7Imports the latest AutoComplete docs");
    } else if (action == "path") {
        WinrtUtils::openSubFolder("Flarial\\Scripts");
    } else if (action == "reload") {
        ScriptManager::reloadScripts();
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

        std::weak_ptr weakSelf = shared_from_this();

        [weakSelf]() -> winrt::fire_and_forget {
            auto self = weakSelf.lock();
            if (!self) co_return;  // The object was destroyed

            bool success = co_await extractAutoComplete();
            if (success) {
                self->addCommandMessage("§aImported the AutoComplete folder successfully.");
                self->addCommandMessage("Press Win + R and paste to set up your Visual Studio Code workspace.");
            } else {
                self->addCommandMessage("§cFailed to import AutoComplete.");
                self->addCommandMessage("§o§7Send your logs to the nearest developer!");
            }
        }();

        std::string clipboardText = "code --install-extension \"sumneko.lua\" & code \"" + scriptsFolder.string() + "\"";
        WinrtUtils::setClipboard(clipboardText);
    } else {
        addCommandMessage("§cUsage: .lua <help/reload/path/import/autocomplete>");
    }
}