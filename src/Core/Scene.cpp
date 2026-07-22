#include "Core/Scene.h"
#include "Components/MeshRenderer.h"

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
    for (SceneObject* obj : m_sceneObjects)
    {
        if (!obj->IsActiveInHierarchy())
            continue;

        MeshRenderer* renderer = obj->GetComponent<MeshRenderer>();
        if (renderer)
            renderer->Draw(viewMatrix, projectionMatrix, viewPos);
    }
}

void Scene::AddObject(SceneObject* object)
{
    m_sceneObjects.push_back(object);
}

void Scene::RemoveObject(SceneObject* object)
{
    for (int i = 0; i < m_sceneObjects.size(); i++)
    {
        if (m_sceneObjects[i] == object)
        {
            RemoveObject(i);
            return;
        }
    }
}

void Scene::RemoveObject(int index)
{
    m_sceneObjects.erase(m_sceneObjects.begin() + index);
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
                    std::cerr << "[SCENE] Error: Parent with id " << obj->GetParentID() << "not found for" << obj->name << std::endl;
            }
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
        renderer->SetSharedMaterial(AssetManager::GetMaterial("DefaultMaterial"));

        obj->AddComponent(COMPONENT_TYPE::TELEMETRY_VIEWER);
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
    obj->transform.SetRotation(glm::vec3(glm::degrees(radX), glm::degrees(radY), glm::degrees(radZ)));

    if (parentObject)
        obj->transform.SetParent(&parentObject->transform, false);

    AddObject(obj);

    for (const ModelNode& childNode : node.children)
        InstantiateModelNode(model, childNode, obj);

    return obj;
}