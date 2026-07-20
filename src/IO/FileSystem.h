#pragma once
#include <iostream>
#include <filesystem>
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

        while (!fs::exists(rootDir / "src" / "Shaders") && rootDir.has_parent_path())
            rootDir = rootDir.parent_path();

        if (!std::filesystem::exists(rootDir / "src" / "Shaders")) {
            std::cerr << "[FILESYSTEM] Can't find shaders dir from: " << rootDir << std::endl;
        } else {
            rootDir = rootDir / "src";
            std::cout << "[FILESYSTEM] shaders dir found from : " << rootDir << std::endl;

            userDir = rootDir / "User";
            std::cout << "[FILESYSTEM] Creating user folders" << std::endl;
            fs::create_directories(userDir / "Models");
            fs::create_directories(userDir / "Textures");
            fs::create_directories(userDir / "Materials");
            fs::create_directories(userDir / "ShadersCode");
            fs::create_directories(userDir / "Shaders");
            fs::create_directories(userDir / "Scenes");
        }
    }

    static std::string GetAssetPath(const std::string& relativePath)
    {
        return (rootDir / relativePath).string();
    }

    static std::string GetVertexShaderPath(const std::string& filename)
    {
        fs::path path = userDir / "ShadersCode" / (filename + ".vert");
        if (fs::exists(path))
            return path.string();

        std::cerr << "[FILE SYSTEM] Vertex Shader not found: " << path << std::endl; 
        return "";
    }

    static std::string GetFragmentShaderPath(const std::string& filename)
    {
        fs::path path = userDir / "ShadersCode" / (filename + ".frag");
        if (fs::exists(path))
            return path.string();
        
        std::cerr << "[FILE SYSTEM] Fragment Shader not found: " << path << std::endl; 
        return "";
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
            std::cerr << "[FILESYSTEM] Import error: File doesn't exist: " << sourceFilePath << std::endl;
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
            std::cerr << "[FILESYSTEM] Import error: " << e.what() << std::endl;
            return false;
        }
    }
};