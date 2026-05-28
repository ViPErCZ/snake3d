#ifndef SNAKE3_BARRELNODE3D_H
#define SNAKE3_BARRELNODE3D_H

#include "Renderer/Opengl/Material/MaterialInstance.h"
#include "Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "Renderer/Opengl/Model/Standard/MeshNode3D.h"

namespace Model {
    class BarrelNode3D final : public MeshNode3D {
    public:
        explicit BarrelNode3D(const std::shared_ptr<Tools::ContextState> &contextState,
                              const std::shared_ptr<Manager::ResourceManager> &resourceManager);

        void init();

        void update(float dt, uint64_t frameId) override;

    protected:
        std::shared_ptr<Material::MaterialInstance> material;
        std::shared_ptr<Feature::AlbedoFeature> albedoFeature;
        std::shared_ptr<Feature::NormalMapFeature> normalFeature;
        bool initialized = false;
    };
} // Model

#endif //SNAKE3_BARRELNODE3D_H
