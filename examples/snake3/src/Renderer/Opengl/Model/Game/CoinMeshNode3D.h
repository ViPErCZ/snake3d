#ifndef SNAKE3_COINMESHNODE3D_H
#define SNAKE3_COINMESHNODE3D_H

#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>

namespace Model {
    class CoinMeshNode3D final : public MeshNode3D {
    public:
        // using MeshNode3D::MeshNode3D;
        explicit CoinMeshNode3D(const std::shared_ptr<Lights::SpotLight> &spotLight,
                                const std::shared_ptr<Tools::ContextState> &contextState, const std::shared_ptr<StandardMesh> &mesh,
                                const std::shared_ptr<Manager::ResourceManager> &resourceManager);

        void update(float dt, uint64_t frameId) override;

    protected:
        double lastTime{};
        std::shared_ptr<Lights::SpotLight> spotLight;
        glm::vec3 lastPos{};
    };
} // Model

#endif //SNAKE3_COINMESHNODE3D_H
