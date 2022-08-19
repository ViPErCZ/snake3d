#include "stdafx.h"
#include "App.h"
#include <iostream>

using namespace std;

int W_WIDTH = 1920;
int W_HEIGHT = 1080;

double lastX = (double)W_WIDTH / 2.0f;
double lastY = (double)W_HEIGHT / 2.0f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double x, double y);
void scroll_callback(GLFWwindow* window, double offsetX, double offsetY);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

App *app = new App(W_WIDTH, W_HEIGHT);

int main(int argc, char *argv[]) {
    /*bool windowed = true;
    SDL_Window *window;                    // Declare a pointer

    if (argc > 1 && strcmp(argv[1], "-w") == 0) {
        windowed = true;
    }

    glutInit(&argc, argv);
    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
    TTF_Init();

    // Create an application window with the following settings:
    if (windowed) {
        window = SDL_CreateWindow(
                "Snake 3D",                  // window title
                SDL_WINDOWPOS_UNDEFINED,           // initial x position
                SDL_WINDOWPOS_UNDEFINED,           // initial y position
                W_WIDTH,                               // width, in pixels
                W_HEIGHT,                               // height, in pixels
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED // flags - see below
        );
    } else {
        window = SDL_CreateWindow(
                "Snake 3D",                  // window title
                SDL_WINDOWPOS_UNDEFINED,           // initial x position
                SDL_WINDOWPOS_UNDEFINED,           // initial y position
                W_WIDTH,                               // width, in pixels
                W_HEIGHT,                               // height, in pixels
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN // flags - see below
        );
    }

    // Check that the window was successfully created
    if (window == nullptr) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // Create an OpenGL context associated with the window.
    SDL_GL_CreateContext(window);

    SDL_Cursor *cursor;
    int32_t cursorData[2] = {0, 0};
    cursor = SDL_CreateCursor((Uint8 *) cursorData, (Uint8 *) cursorData, 8, 8, 4, 4);
    SDL_SetCursor(cursor);
    SDL_Event event;

    App *app = new App(&event, W_WIDTH, W_HEIGHT);
    bool loop = true;

    do {
        app->run();

        SDL_PollEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                loop = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        loop = false;
                        break;
                }
                break;
        }
    } while (loop);

    delete app;

    SDL_FreeCursor(cursor);
    SDL_DestroyRenderer(renderer);
    // Close and destroy the window
    SDL_DestroyWindow(window);

    TTF_Quit();

    // Clean up
    SDL_Quit();

    return 0;*/

    GLFWwindow *window;
    if (!glfwInit()) { return 1; }

    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr << "Error: " << description << '\n';
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Snake 3", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glewInit();

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, W_WIDTH, W_HEIGHT);
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Enables the Stencil Buffer
    glEnable(GL_STENCIL_TEST);
    // Sets rules for outcomes of stecil tests
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    app->Init();

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        app->run();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    delete app;

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//        camera.processKeyboard(FORWARD, deltaTime);
//    }
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//        camera.processKeyboard(BACKWARD, deltaTime);
//    }
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//        camera.processKeyboard(LEFT, deltaTime);
//    }
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//        camera.processKeyboard(RIGHT, deltaTime);
//    }
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
void mouse_callback(GLFWwindow* window, double x, double y)
{
    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    double offsetX = x - lastX;
    double offsetY = lastY - y; // reversed since y-coordinates go from bottom to top

    lastX = x;
    lastY = y;

    //camera.processMouseMovement(offsetX, offsetY);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double offsetX, double offsetY)
{
    //camera.processMouseScroll(offsetY);
}

// glfw: keyboard callback is called
// ----------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        app->processInput(key);
    }
}