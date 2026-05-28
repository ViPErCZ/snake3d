#ifndef SNAKE3_RADARHANDLER_H
#define SNAKE3_RADARHANDLER_H

#include <memory>

#include <snake3d/Handler/BaseKeydownHandle.h>
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>

using namespace Model;
using namespace std;

namespace Handler {

    class RadarHandler : public BaseKeydownHandle {
    public:
        explicit RadarHandler(const shared_ptr<MeshNode3D> &radar);
        void onEventHandler(unsigned int key, int scancode, int action, int mods, float deltaTime) override;
        void onDefaultHandler() override = 0;

    protected:
        shared_ptr<MeshNode3D> radar;
    };

} // Handler

#endif //SNAKE3_RADARHANDLER_H
