#ifndef SNAKE3_RADARHANDLER_H
#define SNAKE3_RADARHANDLER_H

#include "../ItemsDto/Radar.h"
#include "BaseKeydownHandle.h"

using namespace ItemsDto;

namespace Handler {

    class RadarHandler : public BaseKeydownHandle {
    public:
        explicit RadarHandler(Radar *radar);
        void onEventHandler(unsigned int key, int scancode, int action, int mods) override;
        void onDefaultHandler() override = 0;

    protected:
        Radar* radar;
    };

} // Handler

#endif //SNAKE3_RADARHANDLER_H
