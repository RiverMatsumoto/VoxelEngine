//
// Created by river on 6/9/2023.
//

#ifndef VOXELENGINE_SHADERDATA_H
#define VOXELENGINE_SHADERDATA_H
#define NUM_POINT_LIGHTS 4

struct ShaderData
{
    // material
    int materialDiffuse;
    int materialSpecular;
    float materialShininess;

    // directional light
    glm::vec3 dirLightDirection;
    glm::vec3 dirLightAmbient;
    glm::vec3 dirLightDiffuse;
    glm::vec3 dirLightSpecular;

    // point light
    glm::vec3 pntLightPositions[NUM_POINT_LIGHTS];
    glm::vec3 pntLightAmbients[NUM_POINT_LIGHTS];
    glm::vec3 pntLightDiffuses[NUM_POINT_LIGHTS];
    glm::vec3 pntLightSpeculars[NUM_POINT_LIGHTS];
    float pntLightConstants[NUM_POINT_LIGHTS];
    float pntLightLinears[NUM_POINT_LIGHTS];
    float pntLightQuadtratics[NUM_POINT_LIGHTS];

    // spotlight
    glm::vec3 spotLightPosition;
    glm::vec3 spotLightDirection;
    float cutOff;
    float outCutOff;
    glm::vec3 spotLightAmbient;
    glm::vec3 spotLightDiffuse;
    glm::vec3 spotLightSpecular;
    float spotLightConstant;
    float spotLightLinear;
    float spotLightQuadtratic;
};

#endif //VOXELENGINE_SHADERDATA_H
