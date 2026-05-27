#ifndef SNAKE3_SKYBOXNODE3D_H
#define SNAKE3_SKYBOXNODE3D_H

#include "MeshNode3D.h"

namespace Model {
    class SkyboxNode3D : public MeshNode3D {
    public:
        explicit SkyboxNode3D(const std::shared_ptr<ContextState> &contextState,
                              const std::shared_ptr<ResourceManager> &resourceManager, const std::shared_ptr<Camera> &camera);

        void render(const std::shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        void renderShadows(const std::shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                           const glm::mat4 &parentTransform) const override {
        } // no shadows - skybox
    };
} // Model

#endif //SNAKE3_SKYBOXNODE3D_H
