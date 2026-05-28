#ifndef POSITIONHANDLER_H
#define POSITIONHANDLER_H

#include "BaseTransform.h"
#include "../../Manager/Camera.h"
#include "../BaseKeydownHandle.h"

namespace Handler::Debug {
    class PositionHandler final : public BaseKeydownHandle, public ::Debug::BaseTransform {
    public:
        explicit PositionHandler(const std::shared_ptr<Manager::Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;
    };
} // Handler

#endif //POSITIONHANDLER_H
