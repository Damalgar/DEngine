#pragma once

class SceneObject;
class Material;
class Texture;

enum class SELECTION_TYPE {
    NONE = 0,
    SCENE_OBJECT,
    MATERIAL,
    TEXTURE
};

class SelectionManager {
    public:
    static SELECTION_TYPE GetSelectedType() { return m_selectedType; }
    
    static SceneObject* GetAsSceneObject();
    static Material* GetAsMaterial();
    static Texture* GetAsTexture();

    static void Deselect();
    static void Select(SceneObject* obj);
    static void Select(Material* mat);
    static void Select(Texture* text);


    private:
    inline static SELECTION_TYPE m_selectedType = SELECTION_TYPE::NONE;
    inline static void* m_data = nullptr;
};