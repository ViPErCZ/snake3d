#include <stdafx.h>
#include "App.h"
#include "Renderer/Opengl/RenderStats.h"
#include "Tools/BuildSettings.h"
#include <chrono>
#include <iostream>

using namespace std;

int W_WIDTH = 1920;
int W_HEIGHT = 1080;

double lastX = static_cast<double>(W_WIDTH) / 2.0f;
double lastY = static_cast<double>(W_HEIGHT) / 2.0f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double x, double y);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double offsetX, double offsetY);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

auto camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
auto app = make_shared<App>(camera, W_WIDTH, W_HEIGHT);

int main(int argc, char *argv[]) {
    if (!glfwInit()) { return 1; }

    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr << "Error: " << description << '\n';
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Snake 3", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glewInit();

    glfwGetFramebufferSize(window, &W_WIDTH, &W_HEIGHT);
    app->resize(W_WIDTH, W_HEIGHT);

    // Debug build defaultně ukáže systémový kurzor - umožní klikat na ImGui
    // overlay bez RMB hold. Release build ukáže prázdný kurzor (hra má vlastní
    // crosshair / nepoužívá GUI). Camera rotation v debug je gated na
    // Ctrl/RMB v App::mousePositionCallback.
    if constexpr (Build::isDebug) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, W_WIDTH, W_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    app->Init();
    app->initDebugOverlay(window);

    while (!glfwWindowShouldClose(window))
    {
        app->run();

        // glfw: swap buffers and poll IO events. Měřeno - dlouhý swap typicky
        // znamená vsync wait (GPU work nedoběhl). Long pollEvents je rare.
        const auto swapStart = std::chrono::steady_clock::now();
        glfwSwapBuffers(window);
        glfwPollEvents();
        Renderer::RenderStats::swapMs = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - swapStart).count();
    }

    app.reset();
    camera.reset();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// glfw: whenever the window size changed (by OS or user resize), this callback function executes
// ----------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    W_WIDTH = width;
    W_HEIGHT = height;
    if (app) {
        app->resize(width, height);
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double x, double y)
{
    // if (firstMouse)
    // {
    //     lastX = x;
    //     lastY = y;
    //     firstMouse = false;
    // }
    //
    // const double offsetX = x - lastX;
    // const double offsetY = lastY - y; // reversed since y-coordinates go from bottom to top
    //
    // lastX = x;
    // lastY = y;
    //
    // camera->processMouseMovement(offsetX, offsetY);
    app->mousePositionCallback(window, x, y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    app->mouseButtonCallback(window, button, action, mods);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double offsetX, double offsetY)
{
    // camera->processMouseScroll(offsetY);
}

// glfw: keyboard callback is called
// ----------------------------------------------------------------------
void key_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
{
    if (action == GLFW_PRESS) {
        app->setKeyState(key, true);
    } else if (action == GLFW_RELEASE) {
        app->setKeyState(key, false);
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        app->processInput(window, key, scancode, action, mods);
    }

    app->cameraProcessKeyboard(window);
}
