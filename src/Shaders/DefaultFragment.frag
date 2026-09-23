#version 330 core

struct Material {
    vec4 tintColor;
    float shininess;
    int hasColorMap;
    int hasSpecularMap;
};

out vec4 FragColor;

in vec3 fragWorldPos;
in vec2 fragTextCoords;
in vec3 fragNormal;

uniform vec3 viewPos;

uniform Material material;
uniform sampler2D colorMap;
uniform sampler2D specularMap;

uniform bool usePlainColor;

void main()
{
    if (usePlainColor)
    {
        FragColor = tintColor;
        return;
    }
    //===LIGHTNING===

    //variables
    vec4 albedo = material.hasColorMap == 1 ? texture(colorMap, fragTextCoords) * material.tintColor : material.tintColor;
    float specularIntensity = material.hasSpecularMap == 1 ? texture(specularMap, fragTextCoords).r : 1.0;

    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float Kamb = 0.1;

    vec3 N = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.5));
    vec3 L = -lightDir;
    vec3 V = normalize(viewPos - fragWorldPos);
    vec3 H = normalize(V + L);

    //ambient
    vec3 ambientLight = Kamb * albedo.rgb;

    //diffuse
    float diff = max(0.0, dot(N,L));
    vec3 diffLight = albedo.rgb * diff;

    //specular
    float spec = pow(max(0.0, dot(H, N)), material.shininess);
    vec3 specLight = lightColor * (spec * specularIntensity);

    vec3 finalColor = diffLight + ambientLight + specLight;
    FragColor = vec4(finalColor, 1.0);
}