#ifndef SNAKE3_SCALEHANDLER_H
#define SNAKE3_SCALEHANDLER_H

#include <snake3d/Handler/Debug/BaseTransform.h>
#include <snake3d/Handler/BaseKeydownHandle.h>
#include <snake3d/Manager/Camera.h>
#include "Renderer/Opengl/Model/Standard/MeshNode3D.h"

namespace Handler::Debug {
    class ScaleHandler final : public BaseKeydownHandle, public ::Debug::BaseTransform {
    public:
        explicit ScaleHandler(const std::shared_ptr<Manager::Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;
    };
}

#endif //SNAKE3_SCALEHANDLER_H
