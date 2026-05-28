#ifndef POSITIONHANDLER_H
#define POSITIONHANDLER_H

#include <snake3d/Handler/Debug/BaseTransform.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Handler/BaseKeydownHandle.h>

namespace Handler::Debug {
    class PositionHandler final : public BaseKeydownHandle, public ::Debug::BaseTransform {
    public:
        explicit PositionHandler(const std::shared_ptr<Manager::Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;
    };
} // Handler

#endif //POSITIONHANDLER_H
