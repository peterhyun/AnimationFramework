#pragma once
//
//  Renderer.h
//  FBXLoader
//
//  Created by Jeehoon Hyun on 08/07/2019.
//  Copyright ?2019 Jeehoon Hyun. All rights reserved.
//

#ifndef Renderer_h
#define Renderer_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Shader.h"
#include "FBXAssimp.h"
#include "Floor.h"
#include "Axis.h"
#include "depthMap.h"

class Renderer {
private:
    FBXAssimp* fbxAssimp;
    int frameNum;
    void configureOpenGLSettings();
    void compileShader();
    GLFWwindow* window;
    Shader* modelShader;
    Shader* floorShader;
    Shader* depthMapShader;
    Shader* depthMapAnimationShader;
    Shader* debugShader;
    Shader* stencilTestShader;
    Shader* axisShader;
    Floor* ourFloor;
    Axis* ourAxis;
    depthMap* DepthMap;
    int initialize();

    //debugging for depthMaps
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    void renderQuad();

public:
    Renderer(const char* fileName);
    void renderLoop();
};

#endif /* Renderer_h */
