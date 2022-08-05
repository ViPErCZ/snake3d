#ifndef SNAKE3_BASEHANDLER_H
#define SNAKE3_BASEHANDLER_H

#include "../stdafx.h"

namespace Handler {

    class BaseHandler {
    public:
        virtual ~BaseHandler();
        virtual void onDefaultHandler() = 0;
    };

} // Handler

#endif //SNAKE3_BASEHANDLER_H
