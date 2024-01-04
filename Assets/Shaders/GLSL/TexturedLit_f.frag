#version 450 core

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
    float DebugOutState;
    float _pad;
} materialProperties;


layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inFragPos;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 3) uniform sampler2D textureAlbedo;
layout(set = 0, binding = 4) uniform sampler2D textureNormal;

vec3 CalculateViewSpaceNormal(vec3 normal, mat4 cameraView) {
    mat3 normalMatrix = mat3(cameraView);
    return normalize(normalMatrix * normal);
}

vec3 CalculateLightDirection(vec3 lightPosition, vec3 fragmentPosition) {
    return normalize(lightPosition - fragmentPosition);
}

float CalculateDiffuse(vec3 viewSpaceNormal, vec3 lightDirection) {
    return max(dot(viewSpaceNormal, lightDirection), 0.0);
}

vec3 CalculateReflectionDirection(vec3 lightDirection, vec3 viewSpaceNormal) {
    return reflect(-lightDirection, viewSpaceNormal);
}

vec3 CalculateFinalColor(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor) {
    return ambientColor + diffuseColor + specularColor;
}

void main() {

    // TODO: Calculate Tangent normals
    //vec3 texNormal = texture(textureNormal, inUV).xyz * 2.0 - 1.0;
    vec3 normal = normalize(inNormal);
    vec3 lightDirection = CalculateLightDirection(sceneData.light.position, inFragPos);

    float diffuse = max(dot(normal, lightDirection), 0.0);

    vec3 viewDirection = normalize(sceneData.viewPos.xyz - inFragPos);
    vec3 reflectionDirection = reflect(lightDirection, normal);
    float specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialProperties.shininess);

    
    vec3 ambientColor = sceneData.light.ambientStrength * materialProperties.color.rgb;
    vec3 diffuseColor = materialProperties.color.rgb *sceneData.light.color * diffuse;
    vec3 specularColor = materialProperties.specularStrength * specular * sceneData.light.color;
    vec3 finalColor = CalculateFinalColor(ambientColor, diffuseColor, specularColor);
    vec3 texColor = texture(textureAlbedo, inUV).xyz;

    finalColor *= texColor;

    // Cast to int
    int debugSwitchInput = int(round(materialProperties.DebugOutState));

    switch(debugSwitchInput)
    {
        case 1:
            outColor = vec4(normal, 1.0f);
            break;
        case 2:
            outColor = vec4(lightDirection, 1.0f);
            break;
        case 3:
            outColor = vec4(viewDirection, 1.0f);
            break;
        case 4:
            outColor = vec4(reflectionDirection, 1.0f);
            break;
        case 5:
            outColor = vec4(ambientColor, 1.0f);
            break;
        case 6:
            outColor = vec4(diffuseColor, 1.0f);
            break;
        case 7:
            outColor = vec4(specularColor, 1.0f);
            break;
        case 8:
            outColor = vec4(texColor, 1.0f);
            break;
        case 9:
            outColor = vec4(inUV, 0.0f, 1.0f);
            break;
        default:
            outColor = vec4(finalColor, 1.0f);
            break;
    }
}
