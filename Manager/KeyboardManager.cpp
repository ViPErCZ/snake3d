#include "KeyboardManager.h"

namespace Manager {
    KeyboardManager::~KeyboardManager() {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); Iter++) {
            delete (*Iter);
        }
    }

    void KeyboardManager::onKeyPress(int keyCode) {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); Iter++) {
            (*Iter)->onEventHandler(keyCode);
        }
    }

    void KeyboardManager::addEventHandler(BaseKeydownHandle *handler) {
        handlers.push_back(handler);
    }

    void KeyboardManager::runDefault() {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); Iter++) {
            (*Iter)->onDefaultHandler();
        }
    }

} // Manager