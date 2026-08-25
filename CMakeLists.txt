cmake_minimum_required(VERSION 3.10)
project(DEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(src/Vendor/GLFW)

if(MSVC)
    string(REPLACE "/W3" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

add_subdirectory(src/Vendor/assimp)

set(IMGUI_DIR "src/Vendor/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/ImGuizmo.cpp
    ${IMGUI_DIR}/implot_demo.cpp
    ${IMGUI_DIR}/implot.cpp
    ${IMGUI_DIR}/implot_items.cpp
)

include_directories(
    src
    src/Vendor
    src/Core
    src/Tools
    src/Fonts
    src/Shaders
    src/Graphics
    src/IO
    src/Render
    src/Utils
    src/Textures
    src/Components
    src/Physics
    src/UI/Panels
    src/Vendor/glad/include
    src/Vendor/assimp/include
    src/Vendor/glm
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

find_package(OpenGL REQUIRED)

set(SOURCES 
    src/main.cpp

    #===CORE===
    src/Core/Application.cpp
    src/Core/SceneObject.cpp
    src/Core/Scene.cpp
    src/Core/SceneManager.cpp
    src/Core/AssetManager.cpp
    src/Core/AssimpImportManager.cpp
    src/Core/Transform.cpp
    src/Core/TagManager.cpp
    src/Core/TelemetryManager.cpp

    #===IO===
    src/IO/InputsManager.cpp
    src/IO/Console.cpp
    src/IO/TelemetryParser.cpp

    #===GRAPHICS===
    src/Graphics/Camera.cpp
    src/Graphics/PrimitiveFactory.cpp

    #===RENDER===
    src/Render/Texture.cpp
    src/Render/Material.cpp
    src/Render/Framebuffer.cpp
    src/Render/Mesh.cpp

    #===COMPONENTS===
    src/Components/TelemetryViewer.cpp
    src/Components/MeshRenderer.cpp
    
    #===TOOLS===
    src/Tools/FsDialog.cpp

    #===VENDOR===
    src/Vendor/tinyfiledialogs.c
    src/Vendor/glad/src/glad.c

    #===UI===
    src/UI/Editor.cpp
    src/UI/SelectionManager.cpp
    
    #===UI/PANELS===
    src/UI/Panels/UIFileSystem.cpp
    src/UI/Panels/UIConsole.cpp
    src/UI/Panels/UIInspector.cpp
    src/UI/Panels/UITimeline.cpp

    #===ICONS===
    src/Icons/IconsLoader.cpp

    #===UTILS===
    src/Utils/Utils.cpp

    #===IMGUI===
    ${IMGUI_SOURCES}
)

#if(WIN32)
#    list(APPEND SOURCES src/Icons/resource.rc)
#endif()

if(WIN32)
    add_executable(DEngine
        WIN32
        ${SOURCES}
    )
    target_link_libraries(DEngine PRIVATE glfw opengl32 gdi32 shell32 comdlg32 ole32 assimp)
    target_include_directories(DEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Vendor/glm)
    if(MSVC)
        set_target_properties(DEngine PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
        target_compile_options(DEngine PRIVATE /W3 /wd4244 /wd4018 /wd4996)
    endif()
elseif(UNIX AND NOT APPLE)
    add_executable(DEngine 
    ${SOURCES}
    )
    target_link_libraries(DEngine PRIVATE glfw OpenGL::GL dl pthread assimp)
endif()