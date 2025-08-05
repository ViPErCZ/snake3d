#include "KeyboardManager.h"

namespace Manager {
    KeyboardManager::~KeyboardManager() {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); ++Iter) {
            delete (*Iter);
        }
    }

    void KeyboardManager::onKeyPress(const int keyCode, const int scancode, const int action, const int mods) {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); ++Iter) {
            (*Iter)->onEventHandler(keyCode, scancode, action, mods);
        }
    }

    void KeyboardManager::addEventHandler(BaseKeydownHandle *handler) {
        handlers.push_back(handler);
    }

    void KeyboardManager::runDefault() {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); ++Iter) {
            (*Iter)->onDefaultHandler();
        }
    }

} // Manager