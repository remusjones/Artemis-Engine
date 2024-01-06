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
layout(location = 4) in vec3 inTangent;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 3) uniform sampler2DArray textureArray;

vec3 CalculateLightDirection(vec3 lightPosition, vec3 fragmentPosition) {
    return normalize(lightPosition - fragmentPosition);
}
vec3 CalculateFinalColor(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    return ambientColor + diffuseColor + specularColor;
}
void main() {
    int albedoTextureIndex = 0;
    int normalTextureIndex = 1;
    vec3 texNormal = texture(textureArray, vec3(inUV, normalTextureIndex)).xyz * 2.0 - 1.0;
    vec3 T = normalize(inTangent);
    vec3 N = normalize(inNormal);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    vec3 modelNormal = TBN * texNormal;

    // Spec
    vec3 lightDirection = CalculateLightDirection(sceneData.light.position, inFragPos);
    float diffuse = dot(modelNormal, lightDirection) * 0.5 + 0.5;
    vec3 viewDirection = normalize(sceneData.viewPos.xyz - inFragPos);
    vec3 reflectionDirection = reflect(lightDirection, modelNormal);
    float specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialProperties.shininess);

    // Lighting
    vec3 ambientColor = sceneData.light.ambientStrength * materialProperties.color.rgb;
    float distanceToLight = length(sceneData.light.position - inFragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distanceToLight + 0.032 * distanceToLight * distanceToLight);
    vec3 diffuseColor = attenuation * materialProperties.color.rgb * sceneData.light.color * diffuse;
    vec3 specularColor = attenuation * materialProperties.specularStrength * specular * sceneData.light.color;

    vec3 finalColor = CalculateFinalColor(ambientColor, diffuseColor, specularColor);
    vec3 texColor = texture(textureArray, vec3(inUV, albedoTextureIndex)).xyz;
    finalColor *= texColor;

    outColor = vec4(finalColor, 1.0f);
}
