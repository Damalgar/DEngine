#pragma once
#include "imgui.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Icons/materialIcon.h"
#include "Icons/fragmentShaderIcon.h"
#include "Icons/vertexShaderIcon.h"
#include "Icons/shaderIcon.h"
#include "Icons/modelIcon.h"
#include "Icons/sceneIcon.h"
#include "Icons/textureIcon.h"
#include "Icons/BoundingBoxIcon.h"
#include "Icons/Gizmos_TranslateIcon.h"
#include "Icons/Gizmos_RotateIcon.h"
#include "Icons/Gizmos_ScaleIcon.h"

class IconsLoader {
    public:
    inline static GLuint materialIconText = 0;
    inline static GLuint fragmentShaderIconText = 0;
    inline static GLuint vertexShaderIconText = 0;
    inline static GLuint shaderIconText = 0;
    inline static GLuint modelIconText = 0;
    inline static GLuint sceneIconText = 0;
    inline static GLuint textureIconText = 0;
    inline static GLuint boundingBoxIconText = 0;
    inline static GLuint gizmosTranslateIconText = 0;
    inline static GLuint gizmosRotateIconText = 0;
    inline static GLuint gizmosScaleIconText = 0;

    static void LoadIcons(GLFWwindow* window);
    static void LoadAppIcon(GLFWwindow* window);
    static void InstallLinuxDesktopFile();
    static bool LoadTextureFromMemory(const unsigned char* image_data, int data_len, GLuint* out_texture);
    static void loadIcon(GLFWwindow* window);
};