#ifndef SNAKE3_DIRECTIONALLIGHTNODE3D_H
#define SNAKE3_DIRECTIONALLIGHTNODE3D_H

#include <memory>

#include "../Standard/MeshNode3D.h"
#include "LightNode3D.h"

namespace Model {
    class DirectionalLightNode3D : public LightNode3D, public MeshNode3D {
    public:
        DirectionalLightNode3D(const shared_ptr<ContextState> &contextState,
                               const shared_ptr<ShaderManager> &baseShader,
                               const shared_ptr<ResourceManager> &resourceManager);

        ~DirectionalLightNode3D() override;

        void setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) override {
            directionalLight = directional_light;
        };

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

    protected:
        shared_ptr<DirectionalLight> directionalLight;
    };
} // Model

#endif //SNAKE3_DIRECTIONALLIGHTNODE3D_H
