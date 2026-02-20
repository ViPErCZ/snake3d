#ifndef SNAKE3_BASEKEYDOWNHANDLE_H
#define SNAKE3_BASEKEYDOWNHANDLE_H

#include "BaseHandler.h"

namespace Handler {

    class BaseKeydownHandle : BaseHandler {
    public:
        void onDefaultHandler() override = 0;
        virtual void onEventHandler(unsigned int key, int scancode, int action, int mods, float deltaTime) = 0;
    };

} // Handler

#endif //SNAKE3_BASEKEYDOWNHANDLE_H
