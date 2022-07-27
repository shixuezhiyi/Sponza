#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "Shader.hpp"
#include "Model.hpp"

//全局变量
auto SCR_WIDTH = 1280;
auto SCR_HEIGHT = 720;
Camera camera;
bool isFirstMouse = true;
float lastX = SCR_WIDTH / 2.f, lastY = SCR_HEIGHT / 2.f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//
void mouseCallback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (isFirstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        isFirstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    camera.move(MOUSEMOVE, xoffset, yoffset);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.move(FORWARD, 0, 0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.move(BACKWARD, 0, 0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.move(LEFT, 0, 0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.move(RIGHT, 0, 0, deltaTime);
}

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *mainWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (mainWindow == NULL)
    {
        std::cout << "Failed to create GLFW mainWindow" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(mainWindow);
    glfwSetFramebufferSizeCallback(mainWindow,
                                   [](GLFWwindow *window, int w, int h)
                                   { glViewport(0, 0, w, h); });
    glfwSetCursorPosCallback(mainWindow, mouseCallback);
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Shader baseShader("Base");
//    // view与 projection
//    unsigned int vpUBO;
//    glGenBuffers(1, &vpUBO);
//    glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);
//    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
//    glBindBufferBase(GL_UNIFORM_BUFFER, 0, vpUBO);

    Model sponza("sponza-gltf-pbr/sponza.glb");
    while (!glfwWindowShouldClose(mainWindow))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(mainWindow);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        baseShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                                0.1f,
                                                100.0f);
        glm::mat4 view = camera.getViewMat();
        baseShader.setUniform("view", view);
        baseShader.setUniform("projection", projection);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model,
                           glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
        baseShader.setUniform("model", model);
        sponza.draw(baseShader);
        glfwPollEvents();
    }
    glfwSwapBuffers(mainWindow);
    glfwTerminate();
}
