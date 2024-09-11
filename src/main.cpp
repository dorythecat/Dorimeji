#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include "creature/creature.hpp"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); // Make windows transparent
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Remove decorations from windows
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE); // Make windows appear always on top
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Make windows not resizable by the user

    Creature creature1("../assets/test_shime");
    creature1.render();

    glfwTerminate();
    return 0;
}
