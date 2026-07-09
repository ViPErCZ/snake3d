#ifndef SNAKE3_MANIPULATORHANDLER_H
#define SNAKE3_MANIPULATORHANDLER_H

#include <memory>

#include <snake3d/Handler/Debug/CollisionShapeHandler.h>
#include <snake3d/Handler/Debug/LightsHandler.h>
#include <snake3d/Handler/Debug/PositionHandler.h>
#include <snake3d/Handler/Debug/RotationHandler.h>
#include <snake3d/Handler/Debug/ScaleHandler.h>
#include <snake3d/Handler/BaseKeydownHandle.h>
#include <snake3d/Manager/Camera.h>

namespace Handler::Debug {
        class ManipulatorHandler final : public BaseKeydownHandle {
        public:
                explicit ManipulatorHandler(const std::shared_ptr<Tools::ContextState> &contextState,
                        const std::shared_ptr<Manager::ResourceManager> &resourceManager, const std::shared_ptr<Manager::Camera> &camera);
                void onDefaultHandler() override;
                void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;
                [[nodiscard]] std::shared_ptr<PositionHandler> getPositionHandler() const { return positionHandler; }
                [[nodiscard]] std::shared_ptr<ScaleHandler> getScaleHandler() const { return scaleHandler; }
                [[nodiscard]] std::shared_ptr<RotationHandler> getRotationHandler() const { return rotationHandler; }
                [[nodiscard]] std::shared_ptr<CollisionShapeHandler> getCollisionShapeHandler() const { return collisionShapeHandler; }
                [[nodiscard]] std::shared_ptr<LightsHandler> getLightsHandler() const { return lightsHandler; }
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
                std::shared_ptr<PositionHandler> positionHandler;
                std::shared_ptr<ScaleHandler> scaleHandler;
                std::shared_ptr<RotationHandler> rotationHandler;
                std::shared_ptr<CollisionShapeHandler> collisionShapeHandler;
                std::shared_ptr<LightsHandler> lightsHandler;
                bool positionEnable = false;
                bool scaleEnable = false;
                bool rotateEnable = false;
                bool collisionShapeEnable = false;
                bool lightsEnable = false;
        };
}

#endif //SNAKE3_MANIPULATORHANDLER_H