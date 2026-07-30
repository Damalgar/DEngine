#include "Application.h"

#ifdef WIN32
#include <process.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void SetupImGuiStyle();
void StartRenderCycle(GLFWwindow* window);
void EndRenderCycle(GLFWwindow* window);

int main(int argc, char* argv[])
{
    Application app = Application();
    int status = app.Init(argc, argv[0]);
    if (status != 0)
        return status;

    app.MainLoop();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
#ifndef __linux__
    _c_exit();
#endif
    return 0;
}