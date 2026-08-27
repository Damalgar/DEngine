#pragma once

class UIInspector {
    public:
    static void Draw();
    
    private:
    static void DrawSceneObject();
    static void DrawMaterial();
    static void DrawTexture();
    static void DrawShader();
};