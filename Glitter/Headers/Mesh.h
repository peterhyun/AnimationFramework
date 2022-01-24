//
//  Mesh.hpp
//  FBXLoader
//
//  Created by Jeehoon Hyun on 09/07/2019.
//  Copyright ?2019 Jeehoon Hyun. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent = glm::vec3(0.0f);
    glm::uvec4 boneIndices = glm::uvec4(0);
    glm::uvec4 boneIndices2 = glm::uvec4(0);
    glm::vec4 weightIndices = glm::vec4(0.0f);
    glm::vec4 weightIndices2 = glm::vec4(0.0f);
};

class Mesh {
private:
    unsigned int VAO, VBO, EBO;
    void setupMeshBufferObjects();
    //std::string diffuseFilePath;
    //std::string specularFilePath;
    unsigned int diffuseTexture;
    unsigned int specularTexture;
    unsigned int normalTexture;

    //Added 2020/4/17
    const aiTexture * diffuseTextureData;
    const aiTexture * specularTextureData;
    const aiTexture * normalTextureData;

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    void Draw(Shader* ourShader);
    //Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string& diffuseFilePath, std::string& specularFilePath);
    //Added 2020/4/17
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const aiTexture * diffuseTextureData = NULL, const aiTexture * specularTextureData = NULL, const aiTexture * normalTextureData = NULL);
    //const aiTexture * texture;
};

#endif /* Mesh_hpp */
