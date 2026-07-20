cmake_minimum_required(VERSION 3.10)
project(DEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W3 /wd4244 /wd4018 /wd4996)
endif()

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

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
)

include_directories(
    src
    src/Vendor
    src/Core
    src/Tools
    src/Fonts
    src/Shaders
    src/Graphics
    src/Graphics/Mesh
    src/IO
    src/Render
    src/Utils
    src/Textures
    src/Components
    src/Vendor/glad/include
    src/Vendor/assimp/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

find_package(OpenGL REQUIRED)

if(WIN32)
    set(GLFW_DIR "${CMAKE_SOURCE_DIR}/src/Vendor/GLFW")
    include_directories(${GLFW_DIR}/include)
    link_directories(${GLFW_DIR}/lib)
endif()

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})
    link_directories(${GLFW_LIBRARY_DIRS})
endif()

set(SOURCES 
    src/main.cpp

    #===CORE===
    src/Core/Application.cpp
    src/Core/SceneObject.cpp
    src/Core/Scene.cpp
    src/Core/SceneManager.cpp
    src/Core/AssetManager.cpp
    src/Core/AssimpImportManager.cpp

    #===IO===
    src/IO/InputsManager.cpp

    #===GRAPHICS===
    src/Graphics/Camera.cpp

    #===RENDER===
    src/Render/Texture.cpp
    src/Render/Material.cpp
    src/Render/Framebuffer.cpp

    #===COMPONENTS===
    src/Components/Transform.cpp
    src/Components/Mesh.cpp

    #===TOOLS===
    src/Tools/FsDialog.cpp

    #===VENDOR===
    src/Vendor/tinyfiledialogs.c
    src/Vendor/glad/src/glad.c

    #===UI===
    src/UI/Editor.cpp

    #===ICONS===
    src/Icons/IconsLoader.cpp

    #===IMGUI===
    ${IMGUI_SOURCES}
)

if(WIN32)
    list(APPEND SOURCES src/Icons/resource.rc)
endif()

if(WIN32)
    add_executable(DEngine
        WIN32
        ${SOURCES}
    )
    target_link_libraries(DEngine PRIVATE glfw3 opengl32 gdi32 shell32 comdlg32 ole32 assimp)
    if(MSVC)
        set_target_properties(DEngine PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()
elseif(UNIX AND NOT APPLE)
    add_executable(DEngine 
    ${SOURCES}
    )
    target_link_libraries(DEngine PRIVATE ${GLFW_LIBRARIES} OpenGL::GL dl pthread assimp)
endif()