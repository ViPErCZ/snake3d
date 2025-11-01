#include "RadarHandler.h"

namespace Handler {
    RadarHandler::RadarHandler(Radar *radar) : radar(radar) {}

    void RadarHandler::onEventHandler(const unsigned int key, int scancode, const int action, int mods) {
        if (key == GLFW_KEY_R) {
            radar->toggleVisible();
        }
    }

} // Handler