#ifndef SNAKE3_COINMESHNODE3D_H
#define SNAKE3_COINMESHNODE3D_H

#include "../Standard/MeshNode3D.h"

namespace Model {
    class CoinMeshNode3D final : public MeshNode3D {
    public:
        // using MeshNode3D::MeshNode3D;
        explicit CoinMeshNode3D(const shared_ptr<SpotLight> &spotLight,
                                const shared_ptr<ContextState> &contextState, const shared_ptr<StandardMesh> &mesh,
                                const shared_ptr<ResourceManager> &resourceManager);

        void update(float dt, uint64_t frameId) override;

    protected:
        double lastTime{};
        shared_ptr<SpotLight> spotLight;
        glm::vec3 lastPos{};
    };
} // Model

#endif //SNAKE3_COINMESHNODE3D_H
