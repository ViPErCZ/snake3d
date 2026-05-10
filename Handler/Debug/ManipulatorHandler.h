#ifndef SNAKE3_MANIPULATORHANDLER_H
#define SNAKE3_MANIPULATORHANDLER_H

#include <memory>

#include "CollisionShapeHandler.h"
#include "LightsHandler.h"
#include "PositionHandler.h"
#include "RotationHandler.h"
#include "ScaleHandler.h"
#include "../BaseKeydownHandle.h"
#include "../../Manager/Camera.h"

using namespace Manager;

namespace Handler::Debug {
        class ManipulatorHandler final : public BaseKeydownHandle {
        public:
                explicit ManipulatorHandler(const shared_ptr<ContextState> &contextState,
                        const shared_ptr<ResourceManager> &resourceManager, const shared_ptr<Camera> &camera);
                void onDefaultHandler() override;
                void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;
                shared_ptr<PositionHandler> getPositionHandler() { return positionHandler; }
                shared_ptr<ScaleHandler> getScaleHandler() { return scaleHandler; }
                shared_ptr<RotationHandler> getRotationHandler() { return rotationHandler; }
                shared_ptr<CollisionShapeHandler> getCollisionShapeHandler() { return collisionShapeHandler; }
                shared_ptr<LightsHandler> getLightsHandler() { return lightsHandler; }
                [[nodiscard]] bool isActive() const;
                [[nodiscard]] bool isPositionHandlerActive() const { return positionEnable; }
                [[nodiscard]] bool isScaleHandlerActive() const { return scaleEnable; }
                [[nodiscard]] bool isRotationHandlerActive() const { return rotateEnable; }
                [[nodiscard]] bool isCollisionShapeActive() const { return collisionShapeEnable; }
                [[nodiscard]] bool isLightsHandlerActive() const { return lightsEnable; }
                [[nodiscard]] glm::vec3 getItemWorldCenter() const;
                [[nodiscard]] glm::vec3 getWorldMin() const;
                [[nodiscard]] glm::vec3 getWorldMax() const;
        protected:
                shared_ptr<PositionHandler> positionHandler;
                shared_ptr<ScaleHandler> scaleHandler;
                shared_ptr<RotationHandler> rotationHandler;
                shared_ptr<CollisionShapeHandler> collisionShapeHandler;
                shared_ptr<LightsHandler> lightsHandler;
                bool positionEnable = false;
                bool scaleEnable = false;
                bool rotateEnable = false;
                bool collisionShapeEnable = false;
                bool lightsEnable = false;
        };
}

#endif //SNAKE3_MANIPULATORHANDLER_H