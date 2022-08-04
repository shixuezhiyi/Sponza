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
#include "Light.hpp"

//全局变量
const auto SCR_WIDTH = 1280, SCR_HEIGHT = 720;
const auto SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.f, lastY = SCR_HEIGHT / 2.f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool isFirstMouse = true;
string SponzaPath = "Sponza/sponza.gltf";


//
void processInput(GLFWwindow *window, PointLight &light)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDefaultParameters::FORWARD, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDefaultParameters::BACKWARD, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDefaultParameters::LEFT, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDefaultParameters::RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.output();


    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        light.setVisible(true);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
        light.setVisible(false);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

GLFWwindow *setup()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sponza", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    return window;
}

unsigned int buildQuadVAO()
{
    unsigned int quadVAO, quadVBO;
    float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));

    return quadVAO;
}

void renderQuad(unsigned int VAO, Shader &shader)
{
    glBindVertexArray(VAO);
    shader.use();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

auto getShadowMapFBOAndTex()
{
    glCheckError();
    GLuint shadowMapFBO;
    glGenFramebuffers(1, &shadowMapFBO);
    GLuint cubeShadowMap;
    glGenTextures(1, &cubeShadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeShadowMap);
    for (GLuint i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeShadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCheckError();
    return make_tuple(shadowMapFBO, cubeShadowMap);
}

void renderCubeShadowMap(GLuint &FBO, PointLight &light, MyModel &scene, Shader &shader)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    GLfloat aspect = (GLfloat) SHADOW_WIDTH / (GLfloat) SHADOW_HEIGHT;
    GLfloat near = 1.0f;
    GLfloat far = 100.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
    shader.use();
    shader.setUniform("shadowMatrices", light.getShadowTransforms(shadowProj));
    shader.setUniform("lightPos", light.getPos());
    shader.setUniform("farPlane", far);
    scene.draw(shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


int main()
{

    auto mainWindow = setup();
    glEnable(GL_DEPTH_TEST);
    glCheckError();
    Shader baseShader("Base");
    Shader quadShader("QuadBase");
    Shader pbrShader("PBR");
    Shader lightShader("Light");
    Shader cubeShadowShader("../Shaders/SHADOW.vert", "../Shaders/SHADOW.frag", "../Shaders/SHADOW.geom");
//    Shader debugShader("Debug");
    glCheckError();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    MyModel sponza(SponzaPath, model);
    PointLight light;
    auto [shadowFBO, shadowTex] = getShadowMapFBOAndTex();

    while (!glfwWindowShouldClose(mainWindow))
    {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(mainWindow, light);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        renderCubeShadowMap(shadowFBO, light, sponza, cubeShadowShader);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = camera.GetProjectionMatrix((float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 300.0f);
        glm::mat4 view = camera.GetViewMatrix();
        //draw light
        lightShader.setUniform("view", view);
        lightShader.setUniform("projection", projection);
        light.draw(lightShader);
        pbrShader.use();
        pbrShader.setUniform("shadowMap", 4);
        pbrShader.setUniform("farPlane", 100.0f);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTex);
        light.bind(pbrShader);
        pbrShader.setUniform("view", view);
        pbrShader.setUniform("projection", projection);
        sponza.draw(pbrShader);

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }
    glfwTerminate();
}
