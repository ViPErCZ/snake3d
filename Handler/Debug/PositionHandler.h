#ifndef POSITIONHANDLER_H
#define POSITIONHANDLER_H

#include "BaseTransform.h"
#include "../../Manager/Camera.h"
#include "../BaseKeydownHandle.h"
#include "../../Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace Model;
using namespace Manager;
using namespace ::Debug;

namespace Handler::Debug {
    class PositionHandler final : public BaseKeydownHandle, public BaseTransform {
    public:
        explicit PositionHandler(const shared_ptr<Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;
    };
} // Handler

#endif //POSITIONHANDLER_H
