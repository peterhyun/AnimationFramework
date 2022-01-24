#ifndef DEPTHMAP_H
#define DEPTHMAP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class depthMap {
private:
    void setupdepthMapBufferObjects();

public:
    depthMap();
    const unsigned int SHADOW_WIDTH;
    const unsigned int SHADOW_HEIGHT;
    unsigned int depthMapFBO;
    unsigned int depthMapTexture;
};

#endif