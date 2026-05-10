#ifndef SNAKE3_SNAKEMESHNODE3D_H
#define SNAKE3_SNAKEMESHNODE3D_H

#include <functional>

#include "../../../../Physic/Algorithms/CollisionAlgorithms.h"
#include "../../../../Tools/Timer.h"
#include "../../Material/ShaderMaterial.h"
#include "../../Material/Uniform/TimerUniform.h"
#include "../Standard/MeshNode3D.h"
#include "../Standard/SphereMesh.h"

using namespace Tools;
using namespace Uniform;
using namespace Physic;
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

        explicit SnakeMeshNode3D(const shared_ptr<ContextState> &contextState,
                                 const shared_ptr<StandardMesh> &mesh,
                                 const shared_ptr<ResourceManager> &resourceManager,
                                 const shared_ptr<CollisionSystem3D> &collisionSystem
                                 );

        void respawn();

        void crash();

        void setPostCrashRespawnHandler(std::function<void()> handler);

        void setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) override;

        void setSpotLights(const vector<shared_ptr<SpotLight> > &spot_light) override;

        void setPointLights(const vector<shared_ptr<PointLight> > &point_light) override;

        void setDirection(eDIRECTION direction);

        void setBodySegment(bool bodySegment);

        void addTile(eDIRECTION direction);

        [[nodiscard]] eDIRECTION getDirection() const;

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        void renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                           const glm::mat4 &parentTransform) const override;

        bool isReady() const;
        bool isCrashing() const;
        bool isRespawning() const;

        void setCollisionShape(const shared_ptr<CollisionShape3D> &collisionShape);

        void computeWorldMatrix(const glm::mat4 &parentTransform) override;

        void hide();

    private:
        void stopRespawn();
        unique_ptr<Timer> timer;
        shared_ptr<SphereMesh> createTileNode() const;
        shared_ptr<StandardMaterial> tileMaterial;
        shared_ptr<BaseMaterial> headMaterial;
        shared_ptr<ShaderMaterial> crashMaterial;
        shared_ptr<ShaderMaterial> respawnMaterial;
        shared_ptr<ShaderMaterial> headRespawnMaterial;
        shared_ptr<TimerUniform> timerUniform;
        shared_ptr<TimerUniform> timerUniform2;
        shared_ptr<CollisionSystem3D> collisionSystem;
        eDIRECTION direction = NONE;
        bool respawned = false;
        bool bodySegment = false;
        std::function<void()> postCrashRespawnHandler;
    };
} // Model

#endif //SNAKE3_SNAKEMESHNODE3D_H
