#include "Core/Application.h"
#include "UI/Editor.h"
#include "Core/SceneObject.h"
#include "Core/SceneManager.h"
#include "Core/TagManager.h"
#include "Vendor/imgui/ImGuizmo.h"
#include "Vendor/imgui/implot.h"
#include "Core/TelemetryManager.h"

Application::Application()
{
    if (Instance != nullptr && Instance != this)
    {
        std::cerr << "[APPLICATION] double application instance" << std::endl;
        return;
    }

    Instance = this;
}

int Application::Init(int argc, char* argv0)
{
    if (!glfwInit())
        return 1;
    
    m_window = glfwCreateWindow(1920, 1080, "DEngine", NULL, NULL);

    if (m_window == NULL)
        return 1;

    glfwMakeContextCurrent(m_window);
    glfwMaximizeWindow(m_window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    FileSystem::Init(argv0); //needs to be the first class init

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    FontsLoader::InitializeFonts(io);

    SetupImGuiStyle();
    SetupImGuizmoStyle();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    InputsManager::Init(m_window);
    AssetManager::Init();
    TagManager::Init();
    SceneManager::Init();
    IconsLoader::LoadIcons(m_window);

    SetupDefaults();

    AppTime::Update();

    return 0;
}

void Application::SetupDefaults()
{
    m_camera = new Camera();
    m_sceneBuffer = new Framebuffer(800, 600);
    m_editor = new Editor(m_sceneBuffer->GetTextureID());
}

void Application::MainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        StartRenderCycle();
        SceneManager::GetActiveScene()->Update();
        SceneManager::GetActiveScene()->Render(m_viewMatrix, m_projectionMatrix, m_camera->GetPos());
        EndRenderCycle();
    }

    AssetManager::SaveAll();
    SceneManager::GetActiveScene()->SetName("Autosave");
    SceneManager::SaveCurrentScene();

    delete SceneManager::GetActiveScene();
    AssetManager::Clear();
}

void Application::StartRenderCycle()
{
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    glEnable(GL_DEPTH_TEST);
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    AppTime::Update();
    m_camera->Update();

    ImVec2 viewportSize = m_editor->GetViewportSize();
    if (viewportSize.x > 0.0f && viewportSize.y > 0.0f)
    {
        m_sceneBuffer->Rescale((int)viewportSize.x, (int)viewportSize.y);
        m_projectionMatrix = Utils::GetProjectionMatrix(90.0f, viewportSize.x / viewportSize.y, 0.1f, 100.0f);
    }

    m_sceneBuffer->Bind();

    m_viewMatrix = Utils::GetViewMatrix(m_camera->GetPos(), m_camera->GetPos() + m_camera->GetFront(), vec3(0,1,0));

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::EndRenderCycle()
{
    m_sceneBuffer->Unbind();
    glDisable(GL_DEPTH_TEST);
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_editor->DrawPanels();
    NotificationSystem::Render();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
}

void Application::AddModelToScene(const std::string &filename)
{
    Model* model = AssetManager::GetModel(filename);
    if (!model)
    {
        AssetManager::LoadModel(filename);
        model = AssetManager::GetModel(filename);
    }

    if (model)
    {
        const ModelNode* startNode = &model->GetRootNode();
        if (startNode->meshIndices.empty() && startNode->children.size() == 1)
        {
            while (startNode->meshIndices.empty() && startNode->children.size() == 1)
                startNode = &startNode->children[0];
            SceneManager::GetActiveScene()->InstantiateModelNode(model, *startNode, nullptr);
        } else {
            SceneObject* rootObj = SceneManager::GetActiveScene()->InstantiateModelNode(model, model->GetRootNode(), nullptr);
            if (rootObj)
                rootObj->name = model->GetName();
        }
    }
}

void Application::SetupImGuiStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.WindowRounding    = 4.0f;
    style.FrameRounding     = 6.0f;
    style.GrabRounding      = 3.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarSize     = 14.0f;
    style.ScrollbarRounding = 9.0f;
    style.ItemSpacing       = ImVec2(8, 8);
    style.FramePadding      = ImVec2(8, 6);
    style.FrameBorderSize   = 1.0f;
    style.WindowBorderSize  = 1.0f;

    // --- PALETTE DEEP SLATE ---
    
    // Sfondi Principali (Deep Slate scuro)
    colors[ImGuiCol_WindowBg]             = ImVec4(0.07f, 0.09f, 0.15f, 1.00f); // #111827
    colors[ImGuiCol_ChildBg]              = ImVec4(0.12f, 0.16f, 0.22f, 1.00f); // #1F2937
    colors[ImGuiCol_PopupBg]              = ImVec4(0.12f, 0.16f, 0.22f, 0.96f); 

    // Testo (Chiaro, quasi bianco, leggermente desaturato per non sparare)
    colors[ImGuiCol_Text]                 = ImVec4(0.95f, 0.96f, 0.97f, 1.00f); // #F3F4F6
    colors[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.55f, 0.64f, 1.00f); 

    // Elementi Interni / Campi di Input (Slate medio)
    colors[ImGuiCol_FrameBg]              = ImVec4(0.22f, 0.25f, 0.32f, 1.00f); // #374151
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.29f, 0.33f, 0.39f, 1.00f); 
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.35f, 0.39f, 0.47f, 1.00f); 

    // Accenti Core (Indaco e le sue varianti di stato)
    ImVec4 accentColor                    = ImVec4(0.39f, 0.40f, 0.95f, 1.00f); // #6366F1 (Indigo)
    ImVec4 accentHovered                  = ImVec4(0.51f, 0.55f, 0.97f, 1.00f); // Lighter Indigo
    ImVec4 accentActive                   = ImVec4(0.31f, 0.27f, 0.90f, 1.00f); // Deeper Indigo

    // Bottoni
    colors[ImGuiCol_Button]               = ImVec4(0.22f, 0.25f, 0.32f, 1.00f); 
    colors[ImGuiCol_ButtonHovered]        = accentHovered; 
    colors[ImGuiCol_ButtonActive]         = accentActive; 

    // Headers (Sezioni, nodi dell'albero della gerarchia)
    colors[ImGuiCol_Header]               = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.45f); 
    colors[ImGuiCol_HeaderHovered]        = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.80f); 
    colors[ImGuiCol_HeaderActive]         = accentColor; 

    // Controlli (Slider e Checkbox)
    colors[ImGuiCol_SliderGrab]           = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.90f);
    colors[ImGuiCol_SliderGrabActive]     = accentActive; 
    colors[ImGuiCol_CheckMark]            = accentHovered; 
    
    // Bordi e Separatori (Separazione pulita ma low-contrast)
    colors[ImGuiCol_Separator]            = ImVec4(0.16f, 0.20f, 0.28f, 1.00f); 
    colors[ImGuiCol_Border]               = ImVec4(0.16f, 0.20f, 0.28f, 1.00f); 
    colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); 

    // Barre del Titolo dei Pannelli
    colors[ImGuiCol_TitleBg]              = ImVec4(0.12f, 0.16f, 0.22f, 1.00f); 
    colors[ImGuiCol_TitleBgActive]        = ImVec4(0.16f, 0.20f, 0.28f, 1.00f); 
    colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.07f, 0.09f, 0.15f, 0.75f); 

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.07f, 0.09f, 0.15f, 0.60f); 
    colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.22f, 0.25f, 0.32f, 1.00f); 
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.29f, 0.33f, 0.39f, 1.00f); 
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.35f, 0.39f, 0.47f, 1.00f); 

    // Tabs (In stile IDE moderno)
    colors[ImGuiCol_Tab]                  = ImVec4(0.12f, 0.16f, 0.22f, 1.00f); 
    colors[ImGuiCol_TabHovered]           = accentHovered; 
    colors[ImGuiCol_TabActive]            = accentColor; 
    colors[ImGuiCol_TabUnfocused]         = ImVec4(0.09f, 0.11f, 0.17f, 1.00f); 
    colors[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.12f, 0.16f, 0.22f, 1.00f); 
}

void Application::SetupImGuizmoStyle()
{
    ImGuizmo::Style& gizmoStyle = ImGuizmo::GetStyle();

    gizmoStyle.TranslationLineThickness = 5.0f;
    gizmoStyle.TranslationLineArrowSize = 8.0f;

    gizmoStyle.RotationLineThickness = 3.0f;
    gizmoStyle.RotationOuterLineThickness = 4.0f;

    gizmoStyle.ScaleLineThickness = 5.0f;
    gizmoStyle.ScaleLineCircleSize = 8.0f;

    gizmoStyle.Colors[ImGuizmo::DIRECTION_X] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    gizmoStyle.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
    gizmoStyle.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
}

const std::vector<SceneObject*>& Application::GetSceneObjects() const
{
    if (!SceneManager::GetActiveScene())
    {
        static const std::vector<SceneObject*> empty;
        return empty;
    }

    return SceneManager::GetActiveScene()->GetSceneObjects();
}
