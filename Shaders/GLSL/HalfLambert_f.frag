#version 450

layout(push_constant) uniform PushConstants {
    mat4 model;
} inPushConstants;
layout(set = 0, binding = 0) uniform CameraBuffer{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} cameraData;
layout(set = 0, binding = 1) uniform LightingBuffer {
    vec3 position;
    float lightIntensity;
    vec3 color;
    float ambientStrength;
} lightingData;

layout(set = 0, binding = 2) uniform MaterialProperties {
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
    vec3 normal = texture(textureNormal, inUV).xyz * 2.0 - 1.0;

    // Transform the normal from tangent space to view space
    mat3 normalMatrix = mat3(transpose(inverse(cameraData.view)));
    vec3 viewSpaceNormal = normalize(normalMatrix * normal);

    vec3 lightDirection = normalize(lightingData.position - inFragPos);

    float diffuse = max(dot(viewSpaceNormal, lightDirection), 0.0);

    vec3 viewDirection = normalize(-inFragPos);
    vec3 reflectionDirection = reflect(-lightDirection, viewSpaceNormal);
    float specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialProperties.shininess);

    vec3 ambientColor = lightingData.ambientStrength * materialProperties.color.rgb;
    vec3 diffuseColor = materialProperties.color.rgb * lightingData.color * diffuse;
    vec3 specularColor = materialProperties.specularStrength * specular * lightingData.color;

    vec3 finalColor = ambientColor + diffuseColor + specularColor;

    vec3 texColor = texture(textureAlbedo, inUV).xyz;
    finalColor *= texColor;

    outColor = vec4(finalColor, 1.0f);
}
