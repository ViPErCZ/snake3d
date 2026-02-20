#ifndef SNAKE3_MANIPULATORHANDLER_H
#define SNAKE3_MANIPULATORHANDLER_H

#include <memory>

#include "PositionHandler.h"
#include "RotationHandler.h"
#include "ScaleHandler.h"
#include "../BaseKeydownHandle.h"
#include "../../Manager/Camera.h"

using namespace Manager;

namespace Handler::Debug {
        class ManipulatorHandler final : public BaseKeydownHandle {
        public:
                explicit ManipulatorHandler(const shared_ptr<Camera> &camera);
                void onDefaultHandler() override;
                void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;
                shared_ptr<PositionHandler> getPositionHandler() { return positionHandler; }
                shared_ptr<ScaleHandler> getScaleHandler() { return scaleHandler; }
                shared_ptr<RotationHandler> getRotationHandler() { return rotationHandler; }
                [[nodiscard]] bool isActive() const;
                [[nodiscard]] bool isPositionHandlerActive() const { return positionEnable; }
                [[nodiscard]] bool isScaleHandlerActive() const { return scaleEnable; }
                [[nodiscard]] bool isRotationHandlerActive() const { return rotateEnable; }
                [[nodiscard]] glm::vec3 getItemWorldCenter() const;
                [[nodiscard]] glm::vec3 getWorldMin() const;
                [[nodiscard]] glm::vec3 getWorldMax() const;
        protected:
                shared_ptr<PositionHandler> positionHandler;
                shared_ptr<ScaleHandler> scaleHandler;
                shared_ptr<RotationHandler> rotationHandler;
                bool positionEnable = false;
                bool scaleEnable = false;
                bool rotateEnable = false;
        };
}

#endif //SNAKE3_MANIPULATORHANDLER_H