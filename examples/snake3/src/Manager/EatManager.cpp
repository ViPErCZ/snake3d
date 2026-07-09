#include "EatManager.h"

namespace Manager {
    EatManager::EatManager(const shared_ptr<EatLocationHandler> &handler) : handler(handler) {}

    void EatManager::run(const eat_EVENT event) const {
        switch (event) {
            case eatenUp:
                handler->onDefaultHandler();
                break;
            case clean:
                handler->onCleanHandler();
                break;
            case firstPlace:
                handler->onFirstPlaceHandler();
                break;
            case checkPlace:
                handler->onCheckPlaceHandler();
                break;
            case none:
                break;
        }
    }

} // Manager