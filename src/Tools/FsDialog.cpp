#include "FsDialog.h"
namespace FsDialog {
    std::string applyExtension(const std::string& path, const std::string& ext, bool overrideMode)
    {
        fs::path pathFs(path);

        if (pathFs.extension().string() != ext)
        {
            if (overrideMode)
            {
                pathFs.replace_extension(ext);
                return pathFs.string();
            } 
            else
                return pathFs.string() + ext;
        }
        
        return path;
    }

    std::string OpenFileDialog(const char* title, const std::vector<const char*>& filters, const char* filterDesc) {
        const char* path = tinyfd_openFileDialog(
            title, 
            "", 
            static_cast<int>(filters.size()), 
            filters.data(), 
            filterDesc, 
            0
        );
        return path ? std::string(path) : "";
    }

    std::string SaveFileDialog(const char* title, const std::string& defaultPath, const std::vector<const char*>& filters, const char* filterDesc, const std::string& forceExt) {
        const char* path = tinyfd_saveFileDialog(
            title, 
            defaultPath.c_str(), 
            static_cast<int>(filters.size()), 
            filters.data(), 
            filterDesc
        );

        if (path) {
            std::string result(path);
            if (!forceExt.empty()) {
                return applyExtension(result, forceExt, false);
            }
            return result;
        }
        return "";
    }

    std::string OpenFolderDialog(const char* title) {
        const char* path = tinyfd_selectFolderDialog(title, "");
        return path ? std::string(path) : "";
    }

    std::string OpenModelDialog(const char* title) {
        return OpenFileDialog(title, {"*.obj", "*.fbx"}, "Model files (*.obj, *.fbx)");
    }

    std::string OpenImageDialog(const char* title) {
        return OpenFileDialog(title, {"*.png", "*.jpg", "*.jpeg"}, "Image files (*.png, *.jpg, *.jpeg)");
    }    
}