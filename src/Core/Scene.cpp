#include "Core/Scene.h"
#include "Components/MeshRenderer.h"
#include "Core/TagManager.h"
#include "Graphics/PrimitiveFactory.h"
#include "IO/Console.h"
#include <fstream>

Scene::Scene(std::string name)
{
    m_name = name;
}

Scene::~Scene()
{
    for (SceneObject* obj : m_sceneObjects)
        delete obj;
        
    m_sceneObjects.clear();
}

void Scene::Update()
{
    for (SceneObject* obj : m_sceneObjects)
    {
        if (obj->IsActiveInHierarchy())
            obj->Update();
    }
}

void Scene::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos)
{
    m_currentViewMatrix = viewMatrix;
    m_currentProjectionMatrix = projectionMatrix;
    
    for (SceneObject* obj : m_sceneObjects)
    {
        if (!obj->IsActiveInHierarchy() || !TagManager::IsTagVisible(obj->GetTag()))
            continue;

        MeshRenderer* renderer = obj->GetComponent<MeshRenderer>();
        if (renderer)
            renderer->Draw(viewMatrix, projectionMatrix, viewPos);

        const std::vector<std::unique_ptr<Component>>& components = obj->GetComponents();

        if (m_drawGizmos)
        {
            for (const auto& component : components)
                component->OnDrawGizmos(viewMatrix, projectionMatrix, viewPos);
        }
    }
}

void Scene::AddObject(SceneObject* object)
{
    m_sceneObjects.push_back(object);
}

void Scene::RemoveObject(SceneObject* object)
{
    if (!object)
        return;
    
    if (object->transform.GetParent() != nullptr)
        object->transform.SetParent(nullptr);

    std::vector<SceneObject*> sceneObjectsToDelete;
    CollectHierarchy(object, sceneObjectsToDelete);

    std::unordered_set<SceneObject*> set(sceneObjectsToDelete.begin(), sceneObjectsToDelete.end());

    m_sceneObjects.erase(
        std::remove_if(m_sceneObjects.begin(), m_sceneObjects.end(), [&set](SceneObject* obj)
        {
            return set.find(obj) != set.end();
        }),
        m_sceneObjects.end()
    );

    for (auto it = sceneObjectsToDelete.rbegin(); it != sceneObjectsToDelete.rend(); ++it)
    {
        if (*it) 
            (*it)->transform.SetParent(nullptr);
    }

    for (SceneObject* obj : sceneObjectsToDelete)
        delete obj;
}

void Scene::RemoveObject(int index)
{
    if (index >= 0 && index < m_sceneObjects.size())
        RemoveObject(m_sceneObjects[index]);
}

SceneObject* Scene::CreateEmptyObject(SceneObject* parent)
{
    SceneObject* obj = new SceneObject("SceneObject");
    obj->transform.SetParent(&parent->transform, false);
    AddObject(obj);
    return obj;
}

SceneObject* Scene::CreateCubeObject(SceneObject* parent)
{
    Model* cubeModel = AssetManager::GetModel("Primitive_Cube");

    if (!cubeModel)
        return nullptr;

    SceneObject* obj = InstantiateModelNode(cubeModel, cubeModel->GetRootNode(), parent);

    if (obj)
        obj->name = "Cube";

    return obj;
}

void Scene::CollectHierarchy(SceneObject* root, std::vector<SceneObject*>& hierarchy)
{
    hierarchy.push_back(root);

    for (Transform* child : root->transform.GetChildren())
        CollectHierarchy(child->GetSceneObject(), hierarchy);
}

json Scene::ToJson() const
{
    json j;
    j["name"] = m_name;

    json objectsArray = json::array();
    for (SceneObject* obj : m_sceneObjects)
        objectsArray.push_back(obj->ToJson());

    j["sceneObjects"] = objectsArray;

    return j;
}

void Scene::FromJson(const json& j)
{
    for (SceneObject* obj : m_sceneObjects)
        delete obj;
    m_sceneObjects.clear();

    m_name = j.contains("name") ? j["name"].get<std::string>() : "Scene";

    if (j.contains("sceneObjects"))
    {
        std::unordered_map<unsigned int, SceneObject*> objectMap;

        for (const auto& objJson : j["sceneObjects"])
        {
            SceneObject* newObj = new SceneObject();
            newObj->FromJson(objJson);
            
            m_sceneObjects.push_back(newObj);
            objectMap[newObj->GetID()] = newObj;
        }

        for (SceneObject* obj : m_sceneObjects)
        {
            if (obj->GetParentID() != 0)
            {
                auto it = objectMap.find(obj->GetParentID());
                if (it != objectMap.end())
                {
                    SceneObject* parentObj = it->second;
                    obj->transform.SetParent(&parentObj->transform, false);
                }
                else
                    Console::LogError("Parent with id " + std::to_string(obj->GetParentID()) + " not found for " + obj->name, LOG_CATEGORY::SCENE);
            }
        }

        for (SceneObject* obj : m_sceneObjects)
        {
            if (obj->transform.GetParent() == nullptr)
                obj->transform.GetModelMatrix();
        }
    }
}

SceneObject* Scene::InstantiateModelNode(Model* model, const ModelNode& node, SceneObject* parentObject)
{
    SceneObject* obj = new SceneObject(node.name);
    
    if (!node.meshIndices.empty())
    {
        obj->AddComponent(COMPONENT_TYPE::MESH_RENDERER);
        MeshRenderer* renderer = obj->GetComponent<MeshRenderer>();
        if (!renderer)
            return nullptr;

        renderer->SetMeshIndices(node.meshIndices);
        renderer->SetModel(model);
        renderer->SetMaterial(AssetManager::GetMaterial("DefaultMaterial"));
    }
    
    glm::vec3 scale, pos, skew;
    glm::quat rotQuat;
    glm::vec4 persp;
    glm::decompose(node.localTransform, scale, rotQuat, pos, skew, persp);
    
    glm::mat4 rotMat = glm::mat4_cast(rotQuat);
    float radX, radY, radZ;
    glm::extractEulerAngleYXZ(rotMat, radY, radX, radZ);

    obj->transform.SetPosition(pos);
    obj->transform.SetScale(scale);
    obj->transform.SetRotation(rotQuat);

    if (parentObject)
        obj->transform.SetParent(&parentObject->transform, false);

    AddObject(obj);

    for (const ModelNode& childNode : node.children)
        InstantiateModelNode(model, childNode, obj);

    return obj;
}

void Scene::OnTagDeleted(const std::string& tag)
{
    for (SceneObject* obj : m_sceneObjects)
    {
        if (obj->GetTag() != tag)
            continue;

        obj->SetTag("Default");
    }
}

bool Scene::SavePreset(SceneObject* rootObj, const std::string& name)
{
    return AssetManager::CreateNewPreset(rootObj, name);
}

SceneObject* Scene::GetSceneObjectByID(uint64_t ID)
{
    for (SceneObject* obj : m_sceneObjects)
    {
        if (obj->GetID() == ID)
            return obj;
    }
    return nullptr;
}

SceneObject* Scene::InstantiatePreset(const std::string& name, SceneObject* targetMountPoint)
{
    return AssetManager::InstantiatePreset(name, targetMountPoint);
}

void Scene::OnModelDeleted(Model* model)
{
    for (SceneObject* obj: m_sceneObjects)
    {
        MeshRenderer* renderer = obj->GetComponent<MeshRenderer>();
        if (renderer && renderer->GetModel() == model)
            renderer->SetModel(nullptr);
    }
}

void Scene::OnMaterialDeleted(Material* mat)
{
    Material* defMat = AssetManager::GetMaterial("DefaultMaterial");
    
    for (SceneObject* obj: m_sceneObjects)
    {
        MeshRenderer* renderer = obj->GetComponent<MeshRenderer>();
        if (renderer && renderer->GetMaterial() == mat)
            renderer->SetMaterial(defMat);
    }
}