#ifndef SNAKE3_EATMANAGER_H
#define SNAKE3_EATMANAGER_H

#include "../Handler/EatLocationHandler.h"

using namespace Handler;
using namespace std;

namespace Manager {
    class EatManager final {
    public:
        explicit EatManager(const shared_ptr<EatLocationHandler> &handler);

        enum eat_EVENT { none, eatenUp, firstPlace, checkPlace, clean };

        void run(eat_EVENT event = none) const;

    protected:
        shared_ptr<EatLocationHandler> handler;
    };
} // Manager

#endif //SNAKE3_EATMANAGER_H
