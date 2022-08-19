#include "RadarHandler.h"

namespace Handler {
    RadarHandler::RadarHandler(Radar *radar) : radar(radar) {}

    void RadarHandler::onEventHandler(unsigned int key) {
//        if (key == SDLK_m) {
//            radar->toggleVisible();
//        }
    }

    void RadarHandler::onDefaultHandler() {
        BaseKeydownHandle::onDefaultHandler();
    }

} // Handler