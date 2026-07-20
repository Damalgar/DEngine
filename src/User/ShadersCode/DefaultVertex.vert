#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

out vec2 fragTextCoords;
out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
    //M.V.P
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1);
    fragTextCoords = textCoords;
    fragNormal = normalize(normalMatrix * normal);
    fragWorldPos = vec3(modelMatrix * vec4(pos, 1.0));
}