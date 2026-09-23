#pragma once
#include <iostream>
#include <filesystem>
#include <system_error>
#include "IO/Console.h"
namespace fs = std::filesystem;

class FileSystem
{
    inline static fs::path rootDir = ""; //Build
    inline static fs::path userDir = "";

    public:

    static void Init(const char* argv0)
    {
        fs::path exePath = fs::weakly_canonical(fs::path(argv0));
        rootDir = exePath.parent_path();
        userDir = rootDir / "User";

        if (!fs::exists(userDir))
        {
            std::cout << "[FILESYSTEM] Creating user folders" << std::endl;
            fs::create_directories(userDir / "Models");
            fs::create_directories(userDir / "Textures");
            fs::create_directories(userDir / "Materials");
            fs::create_directories(userDir / "ShadersCode");
            fs::create_directories(userDir / "Scenes");
            fs::create_directories(userDir / "Presets");
        }
        else
            std::cout << "[FILESYSTEM] Root directory set to: " << rootDir << std::endl;
    }

    static std::string GetAssetPath(const std::string& relativePath)
    {
        return (rootDir / relativePath).string();
    }

    template <typename... Args>
    static std::string GetAssetPath(Args... args) 
    {
        fs::path result = rootDir;
        (..., (result /= args)); 
        return result.string();
    }

    template <typename... Args>
    static bool ImportAsset(const std::string& sourceFilePath, Args... args)
    {
        fs::path sourcePath(sourceFilePath);

        if (!fs::exists(sourcePath) || !fs::is_regular_file(sourcePath))
        {
            Console::LogError("Import error: File doesn't exist: " + sourceFilePath, LOG_CATEGORY::SYSTEM);
            return false;
        }

        fs::path targetDir = GetAssetPath(args...);

        if (!fs::exists(targetDir))
            fs::create_directories(targetDir);

        fs::path destinationPath = targetDir / sourcePath.filename();

        try 
        {
            fs::copy_file(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
            std::cout << "[FILESYSTEM] File imported succesfully: " << destinationPath.string() << std::endl;
            return true;
        } 
        catch (const fs::filesystem_error& e) 
        {
            Console::LogError(std::string("Import error: ") + e.what(), LOG_CATEGORY::SYSTEM);
            return false;
        }
    }

    static bool DeleteAsset(const std::string& filename, const std::string& fileExt, const std::string& assetFolderName)
    {
        fs::path folder = GetAssetPath("User", assetFolderName);
        if (!fs::exists(folder))
        {
            Console::LogError("Trying to delete " + filename + "\nbut path " + folder.string() + " doesn't exist", LOG_CATEGORY::SYSTEM);
            return false;
        }

        fs::path finalPath = folder / (filename + fileExt);

        if (!fs::exists(finalPath))
        {
            Console::LogError("Trying to delete " + finalPath.string() + " but doesn't exist", LOG_CATEGORY::SYSTEM);
            return false;
        }

        std::error_code ec;
        bool success = fs::remove(finalPath, ec);

        if (!success)
        {
            Console::LogError("Failed to delete " + finalPath.string() + ". Error: " + ec.message(), LOG_CATEGORY::SYSTEM);
            return false;
        }

        Console::LogInfo("Successfully deleted " + finalPath.string(), LOG_CATEGORY::SYSTEM);
        return true;
    }

    static bool AssetAlreadyExists(const std::string& filename, const std::string& fileExt, const std::string& assetFolderName)
    {
        fs::path folder = GetAssetPath("User", assetFolderName);
        if (!fs::exists(folder))
        {
            Console::LogError("Trying to check " + filename + "\nbut path " + folder.string() + " doesn't exist", LOG_CATEGORY::SYSTEM);
            return false;
        }

        fs::path finalPath = folder / (filename + fileExt);

        return fs::exists(finalPath);
    }

    static bool RenameAsset(const std::string& filename, const std::string& newFilename, const std::string& fileExt, const std::string& assetFolderName)
    {
        fs::path folder = GetAssetPath("User", assetFolderName);
        if (!fs::exists(folder))
        {
            Console::LogError("Trying to rename " + filename + "\nbut path " + folder.string() + " doesn't exist", LOG_CATEGORY::SYSTEM);
            return false;
        }

        fs::path oldPath = folder / (filename + fileExt);

        if (!fs::exists(oldPath))
        {
            Console::LogError("Trying to rename " + oldPath.string() + " but doesn't exist", LOG_CATEGORY::SYSTEM);
            return false;
        }

        fs::path newPath = folder / (newFilename + fileExt);

        std::error_code ec;
        fs::rename(oldPath, newPath, ec);
        if (ec)
        {
            Console::LogError("File Rename Failed: " + ec.message(), LOG_CATEGORY::SYSTEM);
            return false;
        }

        return fs::exists(newPath);
    }
};