//
//  FBXAssimp.cpp
//  FBXLoader
//
//  Created by Jeehoon Hyun on 09/07/2019.
//  Copyright ?2019 Jeehoon Hyun. All rights reserved.
//

#include "FBXAssimp.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

FBXAssimp::FBXAssimp(const char* filePath) {
    scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    fileName = std::string(filePath);
    fileName = fileName.substr(0, fileName.find_last_of('.'));
    std::cout << "fileName is " << fileName << std::endl;
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    else {
        m_GlobalInverseTransform = convertAssimpMat4_glmMat4(scene->mRootNode->mTransformation);
        std::cout << "Assimp model successfully loaded!" << std::endl;
    }
    std::cout << "scene has " << scene->mNumMeshes << " mesh(es)." << std::endl;
    //We first load the 'mesh data' = (vertices, indices, textures, joints = bones). We do it in a recursive manner.
    processNode(scene->mRootNode);
    std::cout << "processing node finished" << std::endl;
    //Now we load the animation for each Joints.
    processAnimationData();

    std::cout << "processing animation data finished" << std::endl;
}

void FBXAssimp::processNode(aiNode* node) {
    /*
    std::cout << "node name is " << node->mName.C_Str() << std::endl;
    std::cout << "node has " << node->mNumMeshes << " mesh(es)" << std::endl;
    std::cout << "node has " << node->mNumChildren << " children" << std::endl;
     */
    std::cout << "num of mesh in this node " << node->mNumMeshes << std::endl;
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i]);
    }
}

Mesh FBXAssimp::processMesh(aiMesh* mesh) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    std::cout << "number of indices: " << indices.size() << std::endl;

    /*  
        didn't know assimp does this for you at loading time lol
        //Added 4/17/2020 for normal mapping
        setTangentBasis(vertices, indices);
    */

    //mesh has the bone data stored within the mesh.
    loadBones(mesh);

    //We now load the skinning data for each bone and set the vertices
    loadBoneSkinningData(mesh, vertices);

    //We get the texture data here.
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    if (mesh->mMaterialIndex >= 0) {
        aiString diffuseFilePath;
        aiString specularFilePath;
        aiString normalFilePath;

        material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffuseFilePath);
        material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), specularFilePath);
        material->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), normalFilePath);
        
        //Trying to get embedded texture
        const aiTexture * diffuseTexture = scene->GetEmbeddedTexture(diffuseFilePath.C_Str());
        if(diffuseTexture!=nullptr){
            std::cout << "embedded diffuse texture loaded!" << std::endl;
            std::cout << diffuseTexture->mFilename.C_Str() << std::endl;
            std::cout << diffuseTexture->mHeight << std::endl;
            std::cout << diffuseTexture->mWidth << std::endl;
        }

        const aiTexture* specularTexture = scene->GetEmbeddedTexture(specularFilePath.C_Str());
        if (specularTexture != nullptr) {
            std::cout << "embedded specular texture loaded!" << std::endl;
            std::cout << specularTexture->mFilename.C_Str() << std::endl;
            std::cout << specularTexture->mHeight << std::endl;
            std::cout << specularTexture->mWidth << std::endl;
        }

        const aiTexture* normalTexture = scene->GetEmbeddedTexture(normalFilePath.C_Str());
        if (normalTexture != nullptr) {
            std::cout << "embedded normal texture loaded!" << std::endl;
            std::cout << normalTexture->mFilename.C_Str() << std::endl;
            std::cout << normalTexture->mHeight << std::endl;
            std::cout << normalTexture->mWidth << std::endl;
        }

        return Mesh(vertices, indices, diffuseTexture, specularTexture, normalTexture);
    }
    std::cout << "number of vertices for this mesh: " << vertices.size() << std::endl;
    return Mesh(vertices, indices);
}

void FBXAssimp::loadBones(const aiMesh* mesh) {
    std::cout << "mNumBones for this mesh is: " << mesh->mNumBones << std::endl;
    for (int i = 0; i < mesh->mNumBones; i++) {
        std::string boneName(mesh->mBones[i]->mName.data);
        std::cout << "boneName is " << boneName << std::endl;
        int jointIndex = findJointIndex(boneName);
        if (jointIndex == -1) {
            std::cout << "bone: " << boneName << " not found in the 'Joints' vector" << std::endl;
            Joint joint;
            joint.boneOffset = convertAssimpMat4_glmMat4(mesh->mBones[i]->mOffsetMatrix);
            joint.name = boneName;
            Joints.push_back(joint);
        }
    }
}

void FBXAssimp::loadBoneSkinningData(const aiMesh* mesh, std::vector<Vertex>& vertices) {
    for (int i = 0; i < mesh->mNumBones; i++) {
        std::string boneName(mesh->mBones[i]->mName.data);
        int boneIndex = findJointIndex(boneName);
        //std::cout << "mesh->mBones[" << i << "]->mNumWeights " << mesh->mBones[i]->mNumWeights << std::endl;
        for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            int vertexIndex = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;
            //std::cout << "vertexIndex is " << vertexIndex << " and weight is " << weight << " and boneIndex is " << boneIndex << std::endl;
            if (vertices[vertexIndex].weightIndices.x == 0.0f) {
                //std::cout << "vertexIndex " << vertexIndex << " weight is " << weight << std::endl;
                vertices[vertexIndex].boneIndices.x = boneIndex;
                vertices[vertexIndex].weightIndices.x = weight;
            }
            else if (vertices[vertexIndex].weightIndices.y == 0.0f) {
                //std::cout << "vertexIndex " << vertexIndex << " weight is " << weight << std::endl;
                vertices[vertexIndex].boneIndices.y = boneIndex;
                vertices[vertexIndex].weightIndices.y = weight;
            }
            else if (vertices[vertexIndex].weightIndices.z == 0.0f) {
                //std::cout << "vertexIndex " << vertexIndex << " weight is " << weight << std::endl;
                vertices[vertexIndex].boneIndices.z = boneIndex;
                vertices[vertexIndex].weightIndices.z = weight;
            }
            else if (vertices[vertexIndex].weightIndices.w == 0.0f) {
                //std::cout << "vertexIndex " << vertexIndex << " weight is " << weight << std::endl;
                vertices[vertexIndex].boneIndices.w = boneIndex;
                vertices[vertexIndex].weightIndices.w = weight;
            }
            else if (vertices[vertexIndex].weightIndices2.x == 0.0f) {
                //std::cout << "vertexIndex " << vertexIndex << " weight is " << weight << std::endl;
                vertices[vertexIndex].boneIndices2.x = boneIndex;
                vertices[vertexIndex].weightIndices2.x = weight;
            }
            else if (vertices[vertexIndex].weightIndices2.y == 0.0f) {
                //std::cout << "vertexIndex " << vertexIndex << " weight is " << weight << std::endl;
                vertices[vertexIndex].boneIndices2.y = boneIndex;
                vertices[vertexIndex].weightIndices2.y = weight;
            }
            else if (vertices[vertexIndex].weightIndices2.z == 0.0f) {
                //std::cout << "vertexIndex " << vertexIndex << " weight is " << weight << std::endl;
                vertices[vertexIndex].boneIndices2.z = boneIndex;
                vertices[vertexIndex].weightIndices2.z = weight;
            }
            else if (vertices[vertexIndex].weightIndices2.w == 0.0f){
                //std::cout << "vertexIndex " << vertexIndex << " weight is " << weight << std::endl;
                vertices[vertexIndex].boneIndices2.w = boneIndex;
                vertices[vertexIndex].weightIndices2.w = weight;
            }
        }
    }
}

void FBXAssimp::processAnimationData() {
    //std::cout << "number of animations in this scene: " << scene->mNumAnimations << std::endl; // Should always be 1 for my code
    //double tickspersecond = scene->mAnimations[0]->mTicksPerSecond; //30 FPS
    FPS = (unsigned int)scene->mAnimations[0]->mTicksPerSecond;
    totalDuration = scene->mAnimations[0]->mDuration;

    std::cout << "totalDuration of anim: " << totalDuration << std::endl;

    glm::mat4 identity = glm::mat4(1.0);
    //In case for the samba.fbx file of which the animation duration is 323, there are 324 keyframes for most joints. That's why the following for loop's termination number is totalDuration+1
    for (int i = 0; i < totalDuration + 1; i++) {
        readNodeAnimationDFS(i, scene->mRootNode, identity);
    }
}

void FBXAssimp::readNodeAnimationDFS(int frameTime, const aiNode* node, glm::mat4 parentTransformMatrix) {
    std::string nodeName(node->mName.data);
    const aiAnimation* animation = scene->mAnimations[0];
    glm::mat4 nodeTransformMatrix = convertAssimpMat4_glmMat4(node->mTransformation);
    const aiNodeAnim* nodeAnim = findNodeAnim(animation, nodeName);

    glm::mat4 globalTransformation = parentTransformMatrix;

    //If the node has animation data
    int jointIndex = findJointIndex(nodeName);
    if (nodeAnim) {
        //We find out which joint this node is
        //int jointIndex = findJointIndex(nodeName);
        //We do the interpolation here
        glm::mat4 localScalingMatrix = getLocalScaling(frameTime, nodeAnim);
        glm::mat4 localRotationMatrix = getLocalRotation(frameTime, nodeAnim);
        glm::mat4 localTranslationMatrix = getLocalTranslation(frameTime, nodeAnim);
        //We get the total local transformation matrix here.
        glm::mat4 nodeTransformMatrix = localTranslationMatrix * localRotationMatrix * localScalingMatrix;
        
        /*
        std::cout << nodeName << ": " << jointIndex << std::endl;
        std::cout << "nodeAnim->mNumPositionKeys: " << nodeAnim->mNumPositionKeys << std::endl;
        std::cout << "nodeAnim->mNumScalingKeys: " << nodeAnim->mNumScalingKeys << std::endl;
        std::cout << "nodeAnim->mNumRotationKeys: " << nodeAnim->mNumRotationKeys << std::endl;
        std::cout << std::endl;
        */

        //We multiply it with the parent transformation
        globalTransformation = globalTransformation * nodeTransformMatrix;
        if (jointIndex != -1) {
            Joints[jointIndex].animationMatrices.push_back(m_GlobalInverseTransform * globalTransformation * Joints[jointIndex].boneOffset);
        }
    }
    else if(jointIndex != -1) { //There might be joints where it doesn't have any animation data but have to be rendered
        //std::cout << "nodeAnim is null for nodeName: " << nodeName << std::endl;
        Joints[jointIndex].animationMatrices.push_back(m_GlobalInverseTransform * globalTransformation * Joints[jointIndex].boneOffset);
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        readNodeAnimationDFS(frameTime, node->mChildren[i], globalTransformation);
    }
}

glm::mat4 FBXAssimp::getLocalScaling(int frameTime, const aiNodeAnim* nodeAnim) {
    int numKeyFrames = nodeAnim->mNumScalingKeys;
    //If the total number of frames matches the number of keyframes, we just return the keyframe data.
    if (totalDuration + 1 == numKeyFrames) {
        aiVector3D temp = nodeAnim->mScalingKeys[frameTime].mValue;
        return glm::scale(glm::mat4(1.0), glm::vec3(temp.x, temp.y, temp.z));
    }
    else if (totalDuration + 1 < numKeyFrames) {
        for (int i = 0; i < nodeAnim->mNumScalingKeys; i++) {
            //If the frameTime matches the keyframe's time, we just use the keyframe data.
            if ((double)frameTime - nodeAnim->mScalingKeys[i].mTime < 0.000001) {
                aiVector3D temp = nodeAnim->mScalingKeys[i].mValue;
                return glm::scale(glm::mat4(1.0), glm::vec3(temp.x, temp.y, temp.z));
            }
            //If the frameTime is in between two consecutive keyframe times, we interpolate between them using delta time.
            if ((double)frameTime <= nodeAnim->mScalingKeys[i].mTime) {
                aiVectorKey later = nodeAnim->mScalingKeys[i];
                aiVectorKey first = nodeAnim->mScalingKeys[i - 1];
                aiVector3D laterVal = later.mValue;
                aiVector3D firstVal = first.mValue;
                aiVector3D delta = laterVal - firstVal;
                float factor = (frameTime - first.mTime) / (later.mTime - first.mTime);
                aiVector3D result = firstVal + (factor * delta);
                return glm::scale(glm::mat4(1.0), glm::vec3(result.x, result.y, result.z));
            }
        }
    }
    else {   //less frames than the whole animation frames. Probably just 1 frame. We just return the first frame's data all the time.
        aiVector3D temp = nodeAnim->mScalingKeys[0].mValue;
        return glm::scale(glm::mat4(1.0), glm::vec3(temp.x, temp.y, temp.z));
    }
    return glm::mat4(1.0);
}

glm::mat4 FBXAssimp::getLocalRotation(int frameTime, const aiNodeAnim* nodeAnim) {
    int numKeyFrames = nodeAnim->mNumRotationKeys;
    if (totalDuration + 1 == numKeyFrames) {
        aiQuaternion temp = nodeAnim->mRotationKeys[frameTime].mValue;
        return glm::toMat4(glm::quat(temp.w, temp.x, temp.y, temp.z));
    }
    else if (totalDuration + 1 < numKeyFrames) {
        for (int i = 0; i < nodeAnim->mNumRotationKeys; i++) {
            if ((double)frameTime - nodeAnim->mRotationKeys[i].mTime < 0.000001) {
                aiQuaternion temp = nodeAnim->mRotationKeys[i].mValue;
                return glm::toMat4(glm::quat(temp.w, temp.x, temp.y, temp.z));
            }
            if ((double)frameTime <= nodeAnim->mRotationKeys[i].mTime) {
                aiQuatKey later = nodeAnim->mRotationKeys[i];
                aiQuatKey first = nodeAnim->mRotationKeys[i - 1];
                float factor = (frameTime - first.mTime) / (later.mTime - first.mTime);
                aiQuaternion out;
                aiQuaternion::Interpolate(out, first.mValue, later.mValue, factor);
                return glm::toMat4(glm::quat(out.w, out.x, out.y, out.z));
            }
        }
    }
    else {   //less frames than the whole animation frames. Probably just 1 frame.
        aiQuaternion temp = nodeAnim->mRotationKeys[0].mValue;
        return glm::toMat4(glm::quat(temp.w, temp.x, temp.y, temp.z));
    }
    return glm::mat4(1.0);
}

glm::mat4 FBXAssimp::getLocalTranslation(int frameTime, const aiNodeAnim* nodeAnim) {
    int numKeyFrames = nodeAnim->mNumPositionKeys;
    if (totalDuration + 1 == numKeyFrames) {
        aiVector3D temp = nodeAnim->mPositionKeys[frameTime].mValue;
        return glm::translate(glm::mat4(1.0), glm::vec3(temp.x, temp.y, temp.z));
    }
    else if (totalDuration + 1 < numKeyFrames) {
        for (int i = 0; i < nodeAnim->mNumPositionKeys; i++) {
            if ((double)frameTime - nodeAnim->mPositionKeys[i].mTime < 0.000001) {
                aiVector3D temp = nodeAnim->mPositionKeys[i].mValue;
                return glm::translate(glm::mat4(1.0), glm::vec3(temp.x, temp.y, temp.z));
            }
            if ((double)frameTime <= nodeAnim->mPositionKeys[i].mTime) {
                aiVectorKey later = nodeAnim->mPositionKeys[i];
                aiVectorKey first = nodeAnim->mPositionKeys[i - 1];
                aiVector3D laterVal = later.mValue;
                aiVector3D firstVal = first.mValue;
                aiVector3D delta = laterVal - firstVal;
                float factor = (frameTime - first.mTime) / (later.mTime - first.mTime);
                aiVector3D result = firstVal + (factor * delta);
                return glm::translate(glm::mat4(1.0), glm::vec3(result.x, result.y, result.z));
            }
        }
    }
    else {   //less frames than the whole animation frames. Probably just 1 frame.
        aiVector3D temp = nodeAnim->mPositionKeys[0].mValue;
        return glm::translate(glm::mat4(1.0), glm::vec3(temp.x, temp.y, temp.z));
    }
    return glm::mat4(1.0);
}

void FBXAssimp::Draw(Shader* modelShader) {
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(modelShader);
}

void FBXAssimp::updateAnimation(int frameIndex, Shader* modelShader) {
    std::vector<glm::mat4> jointTransformations;
    for (int j = 0; j < Joints.size(); j++) {
        //std::cout << "updateAnimation: frameIndex got for joint " << j << " is " << frameIndex << std::endl;
        //std::cout << "Joints[" << j << "]: " << Joints[j].name << ", "  << Joints[j].animationMatrices.size() << std::endl;
        jointTransformations.push_back(Joints[j].animationMatrices[frameIndex]);
    }
    modelShader->use();
    modelShader->setMultipleMat4("Animation", jointTransformations.size(), &jointTransformations[0]);
}

int FBXAssimp::findJointIndex(std::string nodeName) {
    for (int i = 0; i < Joints.size(); i++) {
        if (nodeName == Joints[i].name) {
            return i;
        }
    }
    return -1;
}

const aiNodeAnim* FBXAssimp::findNodeAnim(const aiAnimation* animation, std::string nodeName) {
    //std::cout << "number of channels is " << animation->mNumChannels << std::endl; // 52
    for (int i = 0; i < animation->mNumChannels; i++) {
        const aiNodeAnim* temp = animation->mChannels[i];
        //std::cout << "channel's name is " << temp->mNodeName.C_Str() << std::endl;
        std::string aiNodeName(temp->mNodeName.C_Str());
        if (aiNodeName == nodeName) {
            return temp;
        }
    }
    return NULL;
}

unsigned int FBXAssimp::getTotalFrames() {
    return totalDuration + 1;
}

glm::mat4 FBXAssimp::convertAssimpMat4_glmMat4(const aiMatrix4x4& matrix) {
    glm::mat4 m;
    m[0][0] = matrix.a1;
    m[0][1] = matrix.b1;
    m[0][2] = matrix.c1;
    m[0][3] = matrix.d1;

    m[1][0] = matrix.a2;
    m[1][1] = matrix.b2;
    m[1][2] = matrix.c2;
    m[1][3] = matrix.d2;

    m[2][0] = matrix.a3;
    m[2][1] = matrix.b3;
    m[2][2] = matrix.c3;
    m[2][3] = matrix.d3;

    m[3][0] = matrix.a4;
    m[3][1] = matrix.b4;
    m[3][2] = matrix.c4;
    m[3][3] = matrix.d4;
    return m;
}
