#include "EatManager.h"

namespace Manager {
    EatManager::EatManager(EatLocationHandler *handler) : handler(handler) {}

    EatManager::~EatManager() {
        delete handler;
    }

    void EatManager::run(EatManager::eat_EVENT event) {
        switch (event) {
            case eatenUp:
                handler->onDefaultHandler();
                break;
            case firstPlace:
                handler->onFirstPlaceHandler();
                break;
            case checkPlace:
                handler->onCheckPlaceHandler();
            case none:
                break;
        }
    }

} // Manager