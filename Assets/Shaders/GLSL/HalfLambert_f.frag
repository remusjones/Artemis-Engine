#version 450

layout(push_constant) uniform PushConstants {
    mat4 model;
} inPushConstants;

struct Light{
    vec3 position;
    float lightIntensity;
    vec3 color;
    float ambientStrength;
};

layout(set = 0, binding = 0) uniform SceneBuffer {
    Light light;
    mat4 view;
    mat4 viewproj;
    vec4 viewPos;
} sceneData;

layout(set = 0, binding = 1) uniform MaterialProperties {
    vec4 color;
    float specularStrength;
    float shininess;
} materialProperties;



layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inFragPos;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 3) uniform sampler2D textureAlbedo;
layout(set = 0, binding = 4) uniform sampler2D textureNormal;

void main() {
    vec3 normal = normalize(inNormal);

    // Transform the normal from tangent space to view space
    mat3 normalMatrix = mat3(transpose(inverse(sceneData.view)));
    vec3 viewSpaceNormal = normalize(normalMatrix * normal);

    vec3 lightDirection = normalize(sceneData.light.position - inFragPos);

    float diffuse = dot(normal, lightDirection) * 0.5 + 0.5;

    vec3 viewDirection = normalize(sceneData.viewPos.xyz - inFragPos);
    vec3 reflectionDirection = reflect(lightDirection, viewSpaceNormal);
    float specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialProperties.shininess);

    vec3 ambientColor = sceneData.light.ambientStrength * materialProperties.color.rgb;
    vec3 diffuseColor = materialProperties.color.rgb * sceneData.light.color * diffuse;
    vec3 specularColor = materialProperties.specularStrength * specular * sceneData.light.color;

    vec3 finalColor = ambientColor + diffuseColor + specularColor;

    vec3 texColor = texture(textureAlbedo, inUV).xyz;
    finalColor *= texColor;

    outColor = vec4(finalColor, 1.0f);
}
