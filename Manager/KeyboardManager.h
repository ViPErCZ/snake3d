#ifndef SNAKE3_KEYBOARDMANAGER_H
#define SNAKE3_KEYBOARDMANAGER_H

#include "../stdafx.h"
#include "../Handler/SnakeMoveHandler.h"
#include "../Handler/BaseKeydownHandle.h"
#include <vector>

using namespace std;
using namespace Handler;

namespace Manager {

    class KeyboardManager {
    public:
        virtual ~KeyboardManager() = default;
        void addEventHandler(shared_ptr<BaseKeydownHandle> handler);
        void onKeyPress(int keyCode, int scancode, int action, int mods);
        void runDefault();
    protected:
        vector<shared_ptr<BaseKeydownHandle> > handlers;
    };

} // Manager

#endif //SNAKE3_KEYBOARDMANAGER_H
