#ifndef SNAKE3_STREETLAMPNODE3D_H
#define SNAKE3_STREETLAMPNODE3D_H

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
        shared_ptr<StandardMaterial> material1;
        shared_ptr<StandardMaterial> material2;
        shared_ptr<StandardMaterial> material3;
        shared_ptr<ArrayMesh> mesh2;
        shared_ptr<ArrayMesh> mesh3;
        bool initialized = false;
    };
} // Model

#endif //SNAKE3_STREETLAMPNODE3D_H
