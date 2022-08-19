#ifndef SNAKE3_RADARHANDLER_H
#define SNAKE3_RADARHANDLER_H

#include "../ItemsDto/Radar.h"
#include "BaseKeydownHandle.h"

using namespace ItemsDto;

namespace Handler {

    class RadarHandler : public BaseKeydownHandle {
    public:
        explicit RadarHandler(Radar *radar);
        void onEventHandler(unsigned int key) override;
        void onDefaultHandler() override;

    protected:
        Radar* radar;
    };

} // Handler

#endif //SNAKE3_RADARHANDLER_H
