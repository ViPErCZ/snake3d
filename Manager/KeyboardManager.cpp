#include "KeyboardManager.h"

namespace Manager {
    KeyboardManager::KeyboardManager(SDL_Event *event) : event(event) {}

    KeyboardManager::~KeyboardManager() {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); Iter++) {
            delete (*Iter);
        }
    }

    void KeyboardManager::run() {
        switch (event->type) {
            case SDL_KEYDOWN:
                for (auto Iter = handlers.begin(); Iter < handlers.end(); Iter++) {
                    (*Iter)->onEventHandler(event->key.keysym.sym);
                }
                break;
            default:
                for (auto Iter = handlers.begin(); Iter < handlers.end(); Iter++) {
                    (*Iter)->onDefaultHandler();
                }
                break;
        }
    }

    void KeyboardManager::addEventHandler(BaseKeydownHandle* handler) {
        handlers.push_back(handler);
    }

} // Manager