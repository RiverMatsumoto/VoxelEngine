#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
}; 

struct PointLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;
in vec2 TexCoords;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

#define NUM_POINT_LIGHTS 4
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 DirectionalLightCalculations(DirLight light, vec3 normal, vec3 viewDir);
vec3 PointLightCalculations(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 SpotLightCalculations(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float LinearizeDepth(float depth);
bool isWithinClipSpace();

// depth buffer stuff
float near = 0.1;
float far = 100.0;

uniform bool useDebugCam;
uniform bool usePointLights;
uniform bool useDirectionalLight;
uniform bool useSpotLight;

uniform vec4 debugColor;
uniform vec4 normalCamColor;
uniform int camView;
in vec4 debugClipPosition;
in vec3 vertexPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    if (useDirectionalLight)
        result = DirectionalLightCalculations(dirLight, norm, viewDir);

    if (usePointLights)
        for (int i = 0; i < NUM_POINT_LIGHTS; i++)
            result += PointLightCalculations(pointLights[i], norm, FragPos, viewDir);

    if (useSpotLight)
        result += SpotLightCalculations(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);

    if (!useDebugCam) return;
    if (isWithinClipSpace())
		FragColor = debugColor;
}

vec3 DirectionalLightCalculations(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;
    return (ambient + diffuse + specular);
}

vec3 PointLightCalculations(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

    // specular
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


vec3 SpotLightCalculations(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // cone light dampening
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1; // back to NDC (idk what ndc is)
    return (2.0 * near * far) / (far + near - z * (far - near)); 
}

bool isWithinClipSpace()
{
    return (debugClipPosition.x >= -debugClipPosition.w && debugClipPosition.x <= debugClipPosition.w && 
            debugClipPosition.y >= -debugClipPosition.w && debugClipPosition.y <= debugClipPosition.w &&
            debugClipPosition.z >= -debugClipPosition.w && debugClipPosition.z <= debugClipPosition.w &&
            debugClipPosition.z >= -debugClipPosition.w * 0.1 && 
            debugClipPosition.z <= debugClipPosition.w * 100.0);

}
