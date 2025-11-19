#ifndef SNAKE3_SNAKEMESHNODE3D_H
#define SNAKE3_SNAKEMESHNODE3D_H

#include "../../../../Tools/Timer.h"
#include "../../Material/ShaderMaterial.h"
#include "../../Material/Uniform/TimerUniform.h"
#include "../Standard/MeshNode3D.h"
#include "../Standard/SphereMesh.h"

using namespace Tools;
using namespace Uniform;
using namespace std;

namespace Model {
    class SnakeMeshNode3D final : public MeshNode3D {
    public:
        enum eDIRECTION {
            NONE = -1,
            STOP = 0,
            LEFT = 1,
            RIGHT = 2,
            UP = 3,
            DOWN = 4,
            CRASH = 100,
            PAUSE = 200,
        };

        using MeshNode3D::MeshNode3D;

        explicit SnakeMeshNode3D(const shared_ptr<StandardMesh> &mesh,
                                 const shared_ptr<ResourceManager> &resourceManager);

        void respawn();

        void setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) override;

        void setDirection(eDIRECTION direction);

        void stop(bool stop) const;

        void addTile(eDIRECTION direction);

        [[nodiscard]] eDIRECTION getDirection() const;

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        void stopRespawn();

        bool isReady() const;

    private:
        unique_ptr<Timer> timer;
        shared_ptr<SphereMesh> createTileNode() const;
        shared_ptr<StandardMaterial> tileMaterial;
        shared_ptr<ShaderMaterial> respawnMaterial;
        shared_ptr<TimerUniform> timerUniform;
        eDIRECTION direction = NONE;
        bool respawned = false;
    };
} // Model

#endif //SNAKE3_SNAKEMESHNODE3D_H
