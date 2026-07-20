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

//TODO list//
/*
ThumbnailGenerator per i materiali e i modelli
Finire asset manager
inspector modulare
component dataView (sceglie che dato raccogliere e come visualizzarlo sull'oggetto)
drag & drop in scena
Hierarchy parentale con trasformazioni parentali
Rotazione su assi e punti fissi
gizmos

poi
capire come costruire la macchina (farla da 0? farla già tutta su blender?), raccogliere i dati e creare la timeline
sistema di animazioni (keyframe?)
*/