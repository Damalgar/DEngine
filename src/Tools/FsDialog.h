#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include "../Vendor/tinyfiledialogs.h"
namespace fs = std::filesystem;

namespace FsDialog {

    static std::string applyExtension(const std::string& path, const std::string& ext, bool overrideMode = false);
    std::string SaveFileDialog(const char* title, const std::string& defaultPath, const std::vector<const char*>& filters, const char* filterDesc, const std::string& forceExt = "");
    std::string OpenFileDialog(const char* title, const std::vector<const char*>& filters, const char* filterDesc);

    std::string OpenFolderDialog(const char* title = "Select a destination folder");
    std::string OpenModelDialog(const char* title = "Select a model (.obj, .fbx)");
    std::string OpenImageDialog(const char* title = "Select an image (.png, .jpg, .jpeg)");
    std::string OpenCSVDialog(const char* title = "Select a CSV file (.csv)");
}