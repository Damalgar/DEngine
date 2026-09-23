#pragma once
#include "imgui.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Icons/Images/materialIcon.h"
#include "Icons/Images/fragmentShaderIcon.h"
#include "Icons/Images/vertexShaderIcon.h"
#include "Icons/Images/shaderIcon.h"
#include "Icons/Images/modelIcon.h"
#include "Icons/Images/sceneIcon.h"
#include "Icons/Images/textureIcon.h"
#include "Icons/Images/BoundingBoxIcon.h"
#include "Icons/Images/Gizmos_TranslateIcon.h"
#include "Icons/Images/Gizmos_RotateIcon.h"
#include "Icons/Images/Gizmos_ScaleIcon.h"
#include "Icons/Images/InfoIcon.h"
#include "Icons/Images/WarningIcon.h"
#include "Icons/Images/ErrorIcon.h"
#include "Icons/Images/localGizmosIcon.h"
#include "Icons/Images/worldGizmosIcon.h"
#include "Icons/Images/originPivotIcon.h"
#include "Icons/Images/boundingBoxPivotIcon.h"
#include "Icons/Images/prefabIcon.h"

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
    inline static GLuint infoIconText = 0;
    inline static GLuint warningIconText = 0;
    inline static GLuint errorIconText = 0;
    inline static GLuint localGizmosIconText = 0;
    inline static GLuint worldGizmosIconText = 0;
    inline static GLuint originPivotIconText = 0;
    inline static GLuint boundingBoxPivotIconText = 0;
    inline static GLuint prefabIconText = 0;

    static void LoadIcons(GLFWwindow* window);
    static void LoadAppIcon(GLFWwindow* window);
    static void InstallLinuxDesktopFile();
    static bool LoadTextureFromMemory(const unsigned char* image_data, int data_len, GLuint* out_texture);
    static void loadIcon(GLFWwindow* window);
};