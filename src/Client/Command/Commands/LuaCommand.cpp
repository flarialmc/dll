#include "LuaCommand.hpp"

#include <winrt/base.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>

#include <Utils/WinrtUtils.hpp>
#include <Scripting/Scripting.hpp>

#include <shlobj.h>
#include <string>

#include <miniz/miniz.h>
#include <Manager.hpp>

std::string scriptPath() {
    using namespace winrt::Windows::Storage;

    auto roamingFolder = ApplicationData::Current().RoamingFolder();
    std::string basePath = winrt::to_string(roamingFolder.Path());
    std::filesystem::path scriptsDir = std::filesystem::path(basePath) / "Flarial" / "scripts";

    return scriptsDir.string();
}

void removeDirectories(const std::filesystem::path& dir) {
    namespace fs = std::filesystem;

    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_directory()) {
            removeDirectories(entry.path());
        }
    }
    std::error_code ec;
    if (dir != dir.root_path()) {
        fs::remove(dir, ec);
    }
}

void flatten(const std::string& targetPath) {
    namespace fs = std::filesystem;
    fs::path targetDir(targetPath);

    if (fs::exists(targetDir / "main.json")) return;

    fs::path mainJsonDir;
    for (auto& entry : fs::recursive_directory_iterator(targetDir)) {
        if (entry.is_directory() && fs::exists(entry.path() / "main.json")) {
            mainJsonDir = entry.path();
            break;
        }
    }

    if (mainJsonDir.empty()) return;

    for (auto& entry : fs::directory_iterator(mainJsonDir)) {
        fs::path dest = targetDir / entry.path().filename();
        std::error_code ec;
        fs::rename(entry.path(), dest, ec);
        if (ec) {
            Logger::error("Error moving files: {}", ec.message());
            return;
        }
    }

    std::error_code ec;
    fs::remove_all(mainJsonDir, ec);
    if (ec) {
        Logger::error("Error removing directory: {}", ec.message());
    }

    // if main.json was multiple levels deep, call flatten again
    flatten(targetPath);
    removeDirectories(targetDir);
}



winrt::Windows::Foundation::IAsyncAction extract(winrt::Windows::Storage::StorageFile storageFile, std::string destinationFolder) {
    using namespace winrt::Windows::Storage;

    auto localFolder = ApplicationData::Current().LocalFolder();
    auto copiedFile = co_await storageFile.CopyAsync(localFolder, L"temp.zip", NameCollisionOption::ReplaceExisting);

    std::string localZipPath = winrt::to_string(copiedFile.Path());
    Utils::extractFromFile(localZipPath, destinationFolder);
    co_await copiedFile.DeleteAsync();
}

void LuaCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        addCommandMessage("§cUsage: .lua <help/reload/path/import>");
        return;
    }

    std::string action = String::toLower(args[0]);

    if (action == "help") {
        addCommandMessage("§bpath §8- §7Opens the script directory in File Explorer");
        addCommandMessage("§breload §8- §7Reloads all currently loaded scripts");
        addCommandMessage("§bimport §8- §7Imports .zip scripts into the client");
    } else if (action == "path") {
        WinrtUtils::openSubFolder("Flarial\\scripts");
    } else if (action == "reload") {
        ModuleManager::restartModules = true;
        addCommandMessage("§aReloaded scripts!");
    } else if (action == "import") {
        [this]() -> winrt::fire_and_forget {
            using namespace winrt::Windows::Storage;

            const auto selectedFiles = co_await WinrtUtils::pickFiles(L".zip");
            if (selectedFiles.Size() == 0) co_return;

            std::error_code ec;

            for (uint32_t i = 0; i < selectedFiles.Size(); i++) {
                auto storageFile = selectedFiles.GetAt(i);
                auto fileName = winrt::to_string(storageFile.Name());
                std::filesystem::path p(fileName);

                std::string folderName = p.stem().string();
                std::string targetPath = scriptPath() + "\\" + folderName;

                std::filesystem::create_directories(targetPath, ec);
                if (ec) {
                    Logger::error("Failed to create directory {}: {}", targetPath, ec.message());
                    continue;
                }

                try {
                    co_await extract(storageFile, targetPath);
                    flatten(targetPath);
                    Logger::success("Imported script successfully: {}", folderName);
                } catch (const std::exception &e) {
                    Logger::error("Failed to extract zip: {}", e.what());
                }
            }
        }();
    } else {
        addCommandMessage("§cUsage: .lua <help/reload/path/import>");
    }
}