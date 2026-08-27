#pragma once
#include <iostream>
#include <filesystem>
#include "IO/Console.h"
namespace fs = std::filesystem;

class FileSystem
{
    inline static fs::path rootDir = ""; //src
    inline static fs::path userDir = "";
    
    public:

    static void Init(const char* argv0)
    {
        fs::path exePath = fs::weakly_canonical(fs::path(argv0));
        rootDir = exePath.parent_path();
        userDir = rootDir / "User";

        if (!fs::exists(userDir))
        {
            std::cout << "[FILESYSTEM] Creating user folders next to executable..." << std::endl;
            fs::create_directories(userDir / "Models");
            fs::create_directories(userDir / "Textures");
            fs::create_directories(userDir / "Materials");
            fs::create_directories(userDir / "ShadersCode");
            fs::create_directories(userDir / "Scenes");
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
};