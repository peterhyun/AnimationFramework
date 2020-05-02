#include "Renderer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "You need to input a file name" << std::endl;
        return -1;
    }
    const char* fileName = argv[1];
    std::cout << "Reading file: " << fileName << std::endl;

    Renderer renderer(fileName);

    std::cout << "Calling render loop" << std::endl;
    renderer.renderLoop();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
