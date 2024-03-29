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
        virtual ~KeyboardManager();
        void addEventHandler(BaseKeydownHandle* handler);
        void onKeyPress(int keyCode);
        void runDefault();
    protected:
        vector<BaseKeydownHandle*> handlers;
    };

} // Manager

#endif //SNAKE3_KEYBOARDMANAGER_H
