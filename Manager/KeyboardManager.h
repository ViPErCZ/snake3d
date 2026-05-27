#ifndef SNAKE3_KEYBOARDMANAGER_H
#define SNAKE3_KEYBOARDMANAGER_H

#include "../stdafx.h"
#include "../Handler/BaseKeydownHandle.h"
#include <memory>
#include <vector>

namespace Manager {

    class KeyboardManager {
    public:
        virtual ~KeyboardManager() = default;
        void addEventHandler(std::shared_ptr<Handler::BaseKeydownHandle> handler);
        void onKeyPress(int keyCode, int scancode, int action, int mods, float deltaTime);
        void runDefault();
    protected:
        std::vector<std::shared_ptr<Handler::BaseKeydownHandle> > handlers;
    };

} // Manager

#endif //SNAKE3_KEYBOARDMANAGER_H
