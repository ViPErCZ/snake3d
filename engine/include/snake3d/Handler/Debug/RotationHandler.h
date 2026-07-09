#ifndef SNAKE3_ROTATIONHANDLER_H
#define SNAKE3_ROTATIONHANDLER_H

#include <snake3d/Handler/Debug/BaseTransform.h>
#include <snake3d/Handler/BaseKeydownHandle.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>

namespace Handler::Debug {
    class RotationHandler : public BaseKeydownHandle, public ::Debug::BaseTransform {
    public:
        explicit RotationHandler(const std::shared_ptr<Manager::Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;
    };
}

#endif //SNAKE3_ROTATIONHANDLER_H
