#ifndef SNAKE3_STREETLAMPNODE3D_H
#define SNAKE3_STREETLAMPNODE3D_H

#include "../../Material/MaterialInstance.h"
#include "../../Material/Feature/AlbedoFeature.h"
#include "../../Material/Feature/NormalMapFeature.h"
#include "../../Material/Feature/PbrFeature.h"
#include "../Standard/ArrayMesh.h"
#include "../Standard/MeshNode3D.h"

namespace Model {
    class StreetLampNode3D final : public MeshNode3D {
    public:
        explicit StreetLampNode3D(const shared_ptr<ContextState> &contextState,
                                  const shared_ptr<ResourceManager> &resourceManager);

        void init();

        void update(float dt, uint64_t frameId) override;

    protected:
        shared_ptr<Material::MaterialInstance> material1;
        shared_ptr<Material::MaterialInstance> material2;
        shared_ptr<Material::MaterialInstance> material3;
        shared_ptr<Feature::AlbedoFeature> albedo1;
        shared_ptr<Feature::AlbedoFeature> albedo2;
        shared_ptr<Feature::AlbedoFeature> albedo3;
        shared_ptr<Feature::NormalMapFeature> normal1;
        shared_ptr<Feature::NormalMapFeature> normal2;
        shared_ptr<Feature::NormalMapFeature> normal3;
        shared_ptr<Feature::PbrFeature> pbr1;
        shared_ptr<Feature::PbrFeature> pbr2;
        shared_ptr<ArrayMesh> mesh2;
        shared_ptr<ArrayMesh> mesh3;
        bool initialized = false;
    };
} // Model

#endif //SNAKE3_STREETLAMPNODE3D_H
