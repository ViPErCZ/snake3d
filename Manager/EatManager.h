#ifndef SNAKE3_EATMANAGER_H
#define SNAKE3_EATMANAGER_H

#include "../stdafx.h"
#include "../Handler/BaseHandler.h"
#include "../Handler/EatLocationHandler.h"
#include <vector>

using namespace Handler;
using namespace std;

namespace Manager {

    class EatManager {
    public:
        explicit EatManager(EatLocationHandler *handler);
        virtual ~EatManager();
        enum eat_EVENT { none, eatenUp, firstPlace, checkPlace };
        void run(eat_EVENT event = none);
    protected:
        EatLocationHandler* handler;
    };

} // Manager

#endif //SNAKE3_EATMANAGER_H
