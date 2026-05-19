#ifndef SNAKE3_DIRECTIONALLIGHTNODE3D_H
#define SNAKE3_DIRECTIONALLIGHTNODE3D_H

#include <memory>

#include "../Standard/MeshNode3D.h"
#include "LightNode3D.h"

namespace Model {
    class DirectionalLightNode3D : public LightNode3D, public MeshNode3D {
    public:
        DirectionalLightNode3D(const shared_ptr<ContextState> &contextState,
                               const shared_ptr<ShaderProgram> &baseShader,
                               const shared_ptr<ResourceManager> &resourceManager);

        ~DirectionalLightNode3D() override;

        void setOrientableLight(const shared_ptr<OrientableLight> &light) {
            orientableLight = light;
        }
        
        shared_ptr<OrientableLight> getOrientableLight() const { return orientableLight; }

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

    protected:
        shared_ptr<OrientableLight> orientableLight;
    };
} // Model

#endif //SNAKE3_DIRECTIONALLIGHTNODE3D_H
