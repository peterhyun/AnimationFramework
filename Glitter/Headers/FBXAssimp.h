//
//  FBXAssimp.hpp
//  FBXLoader
//
//  Created by Jeehoon Hyun on 09/07/2019.
//  Copyright ?2019 Jeehoon Hyun. All rights reserved.
//

#ifndef FBXAssimp_hpp
#define FBXAssimp_hpp

#include "Mesh.h"
#include "Joint.h"

/*
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
 */

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class FBXAssimp {
private:
    const aiScene* scene;
    unsigned int FPS;
    unsigned int totalDuration;
    std::vector<Mesh> meshes;
    //This matrix is for the conversion between the different axis systems depending on file formats. Some have left hand coordinate systems while some have right hand coordinate systems.
    glm::mat4 m_GlobalInverseTransform;
    std::string fileName;
    std::vector<Joint> Joints;
    Assimp::Importer importer;

    //These are the core functions
    void processNode(aiNode* node);
    Mesh processMesh(aiMesh* mesh);
    void processAnimationData();
    void loadBones(const aiMesh* mesh);
    void loadBoneSkinningData(const aiMesh* mesh, std::vector<Vertex>& vertices);
    void readNodeAnimationDFS(int i, const aiNode* node, glm::mat4 parentTransform);

    //The following functions are more like utility functions
    glm::mat4 convertAssimpMat4_glmMat4(const aiMatrix4x4& matrix);
    const aiNodeAnim* findNodeAnim(const aiAnimation* animation, std::string nodeName);
    glm::mat4 getLocalScaling(int frameTime, const aiNodeAnim* nodeAnim);
    glm::mat4 getLocalRotation(int frameTime, const aiNodeAnim* nodeAnim);
    glm::mat4 getLocalTranslation(int frameTime, const aiNodeAnim* nodeAnim);
    int findJointIndex(std::string nodeName);

public:
    FBXAssimp(const char* filePath);
    void Draw(Shader* modelShader);
    unsigned int getTotalFrames();
    void updateAnimation(int frameIndex, Shader* modelShader);

};
#endif /* FBXAssimp_hpp */
