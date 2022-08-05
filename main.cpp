#include "stdafx.h"
#include "App.h"
#include <iostream>

using namespace std;

int W_WIDTH = 1920;
int W_HEIGHT = 1080;

int main(int argc, char *argv[]) {
    bool windowed = true;
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

    App *app = new App(window, renderer, &event, W_WIDTH, W_HEIGHT);
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

    return 0;
}
