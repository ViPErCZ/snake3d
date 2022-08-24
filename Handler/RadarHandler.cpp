#include "RadarHandler.h"

namespace Handler {
    RadarHandler::RadarHandler(Radar *radar) : radar(radar) {}

    void RadarHandler::onEventHandler(unsigned int key) {
        if (key == GLFW_KEY_M) {
            radar->toggleVisible();
        }
    }

    void RadarHandler::onDefaultHandler() {
        BaseKeydownHandle::onDefaultHandler();
    }

} // Handler