#include "Icons/IconsLoader.h"

#include <string>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

void IconsLoader::LoadIcons(GLFWwindow* window)
{
    LoadTextureFromMemory(materialIcon_png, materialIcon_png_len, &materialIconText);
    LoadTextureFromMemory(fragmentShaderIcon_png, fragmentShaderIcon_png_len, &fragmentShaderIconText);
    LoadTextureFromMemory(vertexShaderIcon_png, vertexShaderIcon_png_len, &vertexShaderIconText);
    LoadTextureFromMemory(shaderIcon_png, shaderIcon_png_len, &shaderIconText);
    LoadTextureFromMemory(modelIcon_png, modelIcon_png_len, &modelIconText);
    LoadTextureFromMemory(sceneIcon_png, sceneIcon_png_len, &sceneIconText);
    LoadTextureFromMemory(textureIcon_png, textureIcon_png_len, &textureIconText);
    LoadTextureFromMemory(BoundingBoxIcon_png, BoundingBoxIcon_png_len, &boundingBoxIconText);
    LoadTextureFromMemory(Gizmos_TranslateIcon_png, Gizmos_TranslateIcon_png_len, &gizmosTranslateIconText);
    LoadTextureFromMemory(Gizmos_RotateIcon_png, Gizmos_RotateIcon_png_len, &gizmosRotateIconText);
    LoadTextureFromMemory(Gizmos_ScaleIcon_png, Gizmos_ScaleIcon_png_len, &gizmosScaleIconText);
    LoadTextureFromMemory(InfoIcon_png, InfoIcon_png_len, &infoIconText);
    LoadTextureFromMemory(WarningIcon_png, WarningIcon_png_len, &warningIconText);
    LoadTextureFromMemory(ErrorIcon_png, ErrorIcon_png_len, &errorIconText);
    //LoadAppIcon(window);
}

/*void IconsLoader::InstallLinuxDesktopFile() {
    const char* homeDir = getenv("HOME");
    if (!homeDir) return;

    std::string dataDir = std::string(homeDir) + "/.local/share";
    std::string iconPath = dataDir + "/icons/DEngine.png";
    std::string desktopFilePath = dataDir + "/applications/DEngine.desktop";

    if (fs::exists(desktopFilePath)) return;

    try {
        fs::create_directories(dataDir + "/icons");
        fs::create_directories(dataDir + "/applications");
        std::ofstream iconFile(iconPath, std::ios::binary);
        if (iconFile.is_open()) {
            iconFile.write(reinterpret_cast<const char*>(AppIcon_png), AppIcon_png_len);
            iconFile.close();
        }
        std::string exePath = fs::canonical("/proc/self/exe").string();

        std::ofstream desktopFile(desktopFilePath);
        if (desktopFile.is_open()) {
            desktopFile << "[Desktop Entry]\n";
            desktopFile << "Version=1.0\n";
            desktopFile << "Type=Application\n";
            desktopFile << "Name=DEngine\n";
            desktopFile << "Comment=DEngine DAW\n";
            desktopFile << "Exec=\"" << exePath << "\"\n";
            desktopFile << "Icon=" << iconPath << "\n";
            desktopFile << "Terminal=false\n";
            desktopFile << "StartupWMClass=DEngine\n";
            desktopFile << "Categories=Graphics;\n";
            desktopFile.close();

            fs::permissions(desktopFilePath, 
                            fs::perms::owner_read | fs::perms::owner_write | fs::perms::owner_exec, 
                            fs::perm_options::add);
            
        }
    } catch (...) {
    }
}

void IconsLoader::LoadAppIcon(GLFWwindow* window)
{
    int width, height, channels;
    unsigned char* pixels = stbi_load_from_memory(AppIcon_png, AppIcon_png_len, &width, &height, &channels, 4);
    
    if (pixels)
    {
        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = pixels;

        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(pixels);
    }
#ifdef __linux__
        InstallLinuxDesktopFile();
#endif
}*/

bool IconsLoader::LoadTextureFromMemory(const unsigned char* image_data, int data_len, GLuint* out_texture) {
    int image_width = 0;
    int image_height = 0;
    
    stbi_set_flip_vertically_on_load(false);
    unsigned char* decoded_data = stbi_load_from_memory(image_data, data_len, &image_width, &image_height, NULL, 4);
    if (decoded_data == NULL) return false;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, decoded_data);
    
    stbi_image_free(decoded_data);
    *out_texture = image_texture;
    return true;
}