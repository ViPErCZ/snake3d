#include "RadarHandler.h"

namespace Handler {
    RadarHandler::RadarHandler(const shared_ptr<MeshNode3D> &radar) : radar(radar) {}

    void RadarHandler::onEventHandler(const unsigned int key, int scancode, const int action, int mods) {
        if (key == GLFW_KEY_R) {
            radar->setVisible(!radar->isVisible());
        }
    }

} // Handler