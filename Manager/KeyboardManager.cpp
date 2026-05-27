#include "KeyboardManager.h"

using namespace std;
using namespace Handler;

namespace Manager {

    void KeyboardManager::onKeyPress(const int keyCode, const int scancode, const int action, const int mods, float deltaTime) {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); ++Iter) {
            (*Iter)->onEventHandler(keyCode, scancode, action, mods, deltaTime);
        }
    }

    void KeyboardManager::addEventHandler(shared_ptr<BaseKeydownHandle> handler) {
        handlers.push_back(std::move(handler));
    }

    void KeyboardManager::runDefault() {
        for (auto Iter = handlers.begin(); Iter < handlers.end(); ++Iter) {
            (*Iter)->onDefaultHandler();
        }
    }

} // Manager