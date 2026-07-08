#ifndef SNAKE3_LITMESHNODE3D_H
#define SNAKE3_LITMESHNODE3D_H

#include <memory>
#include <vector>

#include <snake3d/Lights/DirectionalLight.h>
#include <snake3d/Lights/PointLight.h>
#include <snake3d/Lights/SpotLight.h>
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>

namespace Model {
    class LitMeshNode3D : public MeshNode3D {
    public:
        using MeshNode3D::MeshNode3D;

        virtual void setDirectionalLight(const std::shared_ptr<Lights::DirectionalLight> &directional_light);

        virtual void setSpotLights(const std::vector<std::shared_ptr<Lights::SpotLight> > &spot_light);

        virtual void setPointLights(const std::vector<std::shared_ptr<Lights::PointLight> > &point_light);

    protected:
        std::shared_ptr<Lights::DirectionalLight> directionalLight;
        std::vector<std::shared_ptr<Lights::SpotLight> > spotLights;
        std::vector<std::shared_ptr<Lights::PointLight> > pointLights;
    };
} // Model

#endif //SNAKE3_LITMESHNODE3D_H
