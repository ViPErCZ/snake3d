#ifndef SNAKE3_BARRELNODE3D_H
#define SNAKE3_BARRELNODE3D_H

#include "Renderer/Opengl/Material/MaterialInstance.h"
#include "Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "Renderer/Opengl/Model/Standard/MeshNode3D.h"

namespace Model {
    class BarrelNode3D final : public MeshNode3D {
    public:
        explicit BarrelNode3D(const shared_ptr<ContextState> &contextState,
                              const shared_ptr<ResourceManager> &resourceManager);

        void init();

        void update(float dt, uint64_t frameId) override;

    protected:
        shared_ptr<Material::MaterialInstance> material;
        shared_ptr<Feature::AlbedoFeature> albedoFeature;
        shared_ptr<Feature::NormalMapFeature> normalFeature;
        bool initialized = false;
    };
} // Model

#endif //SNAKE3_BARRELNODE3D_H
