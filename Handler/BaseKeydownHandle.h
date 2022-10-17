#ifndef SNAKE3_BASEKEYDOWNHANDLE_H
#define SNAKE3_BASEKEYDOWNHANDLE_H

#include "BaseHandler.h"

namespace Handler {

    class BaseKeydownHandle : BaseHandler {
    public:
        void onDefaultHandler() override;
        virtual void onEventHandler(unsigned int key) = 0;
    };

} // Handler

#endif //SNAKE3_BASEKEYDOWNHANDLE_H
