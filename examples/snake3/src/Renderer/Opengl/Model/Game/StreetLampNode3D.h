#ifndef SNAKE3_STREETLAMPNODE3D_H
#define SNAKE3_STREETLAMPNODE3D_H

#include <snake3d/Renderer/Opengl/Material/MaterialInstance.h>
#include <snake3d/Renderer/Opengl/Material/Feature/AlbedoFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/NormalMapFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/PbrFeature.h>
#include <snake3d/Renderer/Opengl/Model/Standard/ArrayMesh.h>

#include "Renderer/Opengl/Model/Game/LitMeshNode3D.h"

namespace Model {
    class StreetLampNode3D final : public LitMeshNode3D {
    public:
        explicit StreetLampNode3D(const std::shared_ptr<Tools::ContextState> &contextState,
                                  const std::shared_ptr<Manager::ResourceManager> &resourceManager);

        void init();

        void update(float dt, uint64_t frameId) override;

    protected:
        std::shared_ptr<Material::MaterialInstance> material1;
        std::shared_ptr<Material::MaterialInstance> material2;
        std::shared_ptr<Material::MaterialInstance> material3;
        std::shared_ptr<Feature::AlbedoFeature> albedo1;
        std::shared_ptr<Feature::AlbedoFeature> albedo2;
        std::shared_ptr<Feature::AlbedoFeature> albedo3;
        std::shared_ptr<Feature::NormalMapFeature> normal1;
        std::shared_ptr<Feature::NormalMapFeature> normal2;
        std::shared_ptr<Feature::NormalMapFeature> normal3;
        std::shared_ptr<Feature::PbrFeature> pbr1;
        std::shared_ptr<Feature::PbrFeature> pbr2;
        std::shared_ptr<ArrayMesh> mesh2;
        std::shared_ptr<ArrayMesh> mesh3;
        bool initialized = false;
    };
} // Model

#endif //SNAKE3_STREETLAMPNODE3D_H
