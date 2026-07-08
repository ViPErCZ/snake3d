#ifndef SNAKE3_DIRECTIONALLIGHTNODE3D_H
#define SNAKE3_DIRECTIONALLIGHTNODE3D_H

#include <memory>

#include <snake3d/Lights/OrientableLight.h>
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>
#include <snake3d/Renderer/Opengl/Model/Debug/LightNode3D.h>

namespace Model {
    class DirectionalLightNode3D : public LightNode3D, public MeshNode3D {
    public:
        DirectionalLightNode3D(const std::shared_ptr<Tools::ContextState> &contextState,
                               const std::shared_ptr<Manager::ShaderProgram> &baseShader,
                               const std::shared_ptr<Manager::ResourceManager> &resourceManager);

        ~DirectionalLightNode3D() override;

        void setOrientableLight(const std::shared_ptr<Lights::OrientableLight> &light) {
            orientableLight = light;
        }

        std::shared_ptr<Lights::OrientableLight> getOrientableLight() const { return orientableLight; }

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

    protected:
        std::shared_ptr<Lights::OrientableLight> orientableLight;
    };
} // Model

#endif //SNAKE3_DIRECTIONALLIGHTNODE3D_H
