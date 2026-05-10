#ifndef SNAKE3_MARKRINGNODE3D_H
#define SNAKE3_MARKRINGNODE3D_H

#include <memory>

#include "../Standard/MeshNode3D.h"
#include "../../../../Handler/Debug/ManipulatorHandler.h"
#include "../../Material/ShaderMaterial.h"
#include "../../Material/Uniform/CallbackUniform.h"

using namespace Uniform;
using namespace Handler::Debug;
using namespace std;

namespace Model {
    class MarkRingNode3D : public MeshNode3D {
    public:
        explicit MarkRingNode3D(
            const shared_ptr<ContextState> &contextState,
            const shared_ptr<ResourceManager> &resourceManager,
            const shared_ptr<ManipulatorHandler> &manipulatorHandler);

        void init();

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        void renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
            const glm::mat4 &parentTransform) const override;

    private:
        shared_ptr<ShaderMaterial> material;
        shared_ptr<ManipulatorHandler> manipulatorHandler;
    };
} // Model

#endif //SNAKE3_MARKRINGNODE3D_H
