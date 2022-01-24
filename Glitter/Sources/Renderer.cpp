//
//  Renderer.cpp
//  FBXLoader
//
//  Created by Jeehoon Hyun on 08/07/2019.
//  Copyright ?2019 Jeehoon Hyun. All rights reserved.
//

#include "Renderer.h"
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;

//light direction
glm::vec3 lightPos = glm::vec3(500.0f, 1000.0f, 300.0f);

float near_plane = 50.0f;
float far_plane = 4300.0f;
glm::mat4 lightProjection = glm::ortho(-1020.0f, 1020.0f, -320.0f, 320.0f, near_plane, far_plane);
glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 lightSpaceMatrix = lightProjection * lightView;

//These are all parameters for controlling the FPS.
double deltaTime2 = 0;
double lastFrame = 0;
//Adjust your FPS here as much as you want.
double FPS = 30.0;
double limitFPS = 1.0 / FPS;

// Animation
bool playAnimation = false;
float animationStart = 0.0;
int frameIndex = 0;

bool fullScreen = false;
bool wireFrame = false;

//This is for the stencilBuffer
int objectCount = 1;
int chosenStencilValue = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

Renderer::Renderer(const char* fileName) {
    initialize();
    compileShader();
    std::cout << "Shader compile successful." << std::endl;
    fbxAssimp = new FBXAssimp(fileName);
    std::cout << "fbx loading finished." << std::endl;
    ourFloor = new Floor;
    ourAxis = new Axis;
    DepthMap = new depthMap;
    frameNum = fbxAssimp->getTotalFrames();
    std::cout << "frameNum: " << frameNum << std::endl;
}
int Renderer::initialize() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Framework", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialimodelShaderze GLAD" << std::endl;
        return -1;
    }

    configureOpenGLSettings();
    return 0;
}
void Renderer::configureOpenGLSettings() {
    // configure global opengl state
    // -----------------------------modelShader
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glLineWidth(4.0f);
}
void Renderer::compileShader() {
    //Shaders for ordinary rendering
    modelShader = new Shader("model_loading.vs", "model_loading.fs");
    modelShader->use();
    floorShader = new Shader("floor.vs", "floor.fs");
    floorShader->use();
    floorShader->setInt("shadowMap", 0);

    axisShader = new Shader("axis.vs", "axis.fs");
    axisShader->use();

    //Shaders for depthmap. (light's perspective)
    depthMapShader = new Shader("depthMap.vs", "depthMap.fs");
    depthMapShader->use();
    depthMapShader->setInt("depthMap", 0);
    depthMapAnimationShader = new Shader("depthAnimation.vs", "depthAnimation.fs");
    depthMapAnimationShader->use();

    //Shaders for stencil testing
    stencilTestShader = new Shader("stencilShader.vs", "singleColor.fs");
    stencilTestShader->use();

    debugShader = new Shader("debug.vs", "debug.fs");
    debugShader->use();
}

void Renderer::renderLoop() {
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        deltaTime2 += (currentFrame - lastFrame) / limitFPS;
        lastFrame = currentFrame;

        //std::cout << deltaTime2 << std::endl;

        while (deltaTime2 >= 1.0) {
            glm::mat4 model = glm::mat4(1.0);
            glm::mat4 projection = glm::perspective(glm::radians(camera.zoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2500.0f);
            glm::mat4 view = camera.getViewMatrix();

            depthMapShader->use();
            depthMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
            depthMapShader->use();
            depthMapShader->setMat4("model", model);
            depthMapAnimationShader->use();
            depthMapAnimationShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
            depthMapAnimationShader->setMat4("model", model);
            depthMapAnimationShader->setFloat("signal", animationStart);

            //std::cout << "depthMapAnimationShader all set " << std::endl;

            stencilTestShader->use();
            stencilTestShader->setMat4("model", model);
            stencilTestShader->setMat4("projection", projection);
            stencilTestShader->setMat4("view", view);
            stencilTestShader->setFloat("signal", animationStart);

            //std::cout << "stencilTestShader all set" << std::endl;

            modelShader->use();
            modelShader->setMat4("model", model);
            modelShader->setMat4("projection", projection);
            modelShader->setMat4("view", view);
            modelShader->setVec3("lightPos", lightPos);
            modelShader->setFloat("signal", animationStart);
            modelShader->setVec3("viewPos", camera.position());

            //std::cout << "modelShader all set" << std::endl;

            fbxAssimp->updateAnimation(frameIndex, depthMapAnimationShader);
            fbxAssimp->updateAnimation(frameIndex, modelShader);
            fbxAssimp->updateAnimation(frameIndex, stencilTestShader);
            if (playAnimation) {
                frameIndex++;
                if (frameIndex == frameNum)
                    frameIndex = 0;
            }

            //std::cout << "fbxAssimp frameIndex updated to " << frameIndex << std::endl;

            floorShader->use();
            floorShader->setMat4("projection", projection);
            floorShader->setMat4("view", view);
            floorShader->setVec3("viewPos", camera.position());
            floorShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

            //std::cout << "floorShader all set" << std::endl;

            axisShader->use();
            axisShader->setMat4("projection", projection);
            axisShader->setMat4("view", view);

            deltaTime2--;
        }

        //1. Making the depth map
        glViewport(0, 0, DepthMap->SHADOW_WIDTH, DepthMap->SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, DepthMap->depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);// You have to clear AFTER binding the frame buffer
        fbxAssimp->Draw(depthMapAnimationShader);
        ourFloor->draw(depthMapShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //2. Drawing the floor WITHOUT updating the stencil buffer
        //DO NOT WRITE TO THE STENCIL BUFFER WHEN DRAWING FLOOR
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glStencilMask(0x00);
        floorShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DepthMap->depthMapTexture);
        ourFloor->draw(floorShader);
        //Also draw axis
        axisShader->use();
        ourAxis->draw(axisShader);

        //3. Draw objects as normal. Update the stencil buffer as 'objectCount'
        glStencilFunc(GL_ALWAYS, objectCount, 0xFF);
        glStencilMask(0xFF);
        fbxAssimp->Draw(modelShader);

        //4. Draw the outline if chosen
        if (chosenStencilValue == 1) {
            glStencilFunc(GL_NOTEQUAL, objectCount, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            fbxAssimp->Draw(stencilTestShader);
        }

        //Always reset the stencil settings to basic after drawing the outline
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    bool left_shift_pressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    bool mouse_left_pressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    if (left_shift_pressed && mouse_left_pressed) {
        camera.translate(xoffset, yoffset);
    }
    else if (mouse_left_pressed) {
        camera.mouseInput(xoffset, yoffset);
    }
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.mouseScrollInput(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        int screen_w, screen_h;
        //virtual screen size
        glfwGetWindowSize(window, &screen_w, &screen_h);
        int pixel_w, pixel_h;
        //pixel screen size
        glfwGetFramebufferSize(window, &pixel_w, &pixel_h);
        double xpos, ypos;
        //Get the position from screen (virtual screen)
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec2 screen_pos = glm::vec2(xpos, ypos);
        //Change screen pos to pixel pos
        glm::vec2 pixel_pos = screen_pos * glm::vec2(pixel_w, pixel_h) / glm::vec2(screen_w, screen_h);
        //Shift to GL's center convention
        pixel_pos = pixel_pos + glm::vec2(0.5f, 0.5f);
        //gl's coordinate convention starts from the bottom.
        glm::vec3 win = glm::vec3(pixel_pos.x, pixel_h - 1 - pixel_pos.y, 0.0f);
        //Read the depth value here. Is gonna be from 0 to 1
        glReadPixels((GLint)win.x, (GLint)win.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &win.z);
        GLbyte color[4];
        glReadPixels((GLint)win.x, (GLint)win.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        glm::vec4 viewport(0.0f, 0.0f, (float)pixel_w, (float)pixel_h);

        glm::vec3 world = glm::unProject(win, camera.getViewMatrix(), projection, viewport);

        GLuint stencilValue;
        glReadPixels((GLint)win.x, (GLint)win.y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &stencilValue);

        std::cout << "world " << world.x << " " << world.y << " " << world.z << std::endl;

        std::cout << "color " << +(unsigned char)color[0] << " " << +(unsigned char)color[1] << " " << +(unsigned char)color[2] << " " << +(unsigned char)color[3] << std::endl;

        chosenStencilValue = stencilValue;
        std::cout << "stencilValue " << stencilValue << std::endl;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_F && action == GLFW_RELEASE)
    {
        fullScreen = !fullScreen;
        if (fullScreen) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
        }
        else {
            glfwSetWindowMonitor(window, nullptr, 0, 0, SCR_WIDTH, SCR_HEIGHT, 0);
        }
    }
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        wireFrame = !wireFrame;
        if (wireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        playAnimation = !playAnimation;
        animationStart = 1.0;
        std::cout << "Animation Start" << std::endl;
    }
    if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
        playAnimation = false;
        animationStart = 0.0;
        frameIndex = 0;
    }
}


/* //This function is for debugging!
void Renderer::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
*/