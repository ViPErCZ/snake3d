#ifndef SNAKE3_SNAKEMESHNODE3D_H
#define SNAKE3_SNAKEMESHNODE3D_H

#include <functional>

#include "Physic/Algorithms/CollisionAlgorithms.h"
#include "Tools/Timer.h"
#include "Renderer/Opengl/Material/MaterialInstance.h"
#include "Renderer/Opengl/Material/ShaderMaterial.h"
#include "Renderer/Opengl/Material/Feature/LightingFeature.h"
#include "Renderer/Opengl/Material/Uniform/TimerUniform.h"
#include "Renderer/Opengl/Model/Standard/MeshNode3D.h"
#include "Renderer/Opengl/Model/Standard/SphereMesh.h"

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

        explicit SnakeMeshNode3D(const std::shared_ptr<Tools::ContextState> &contextState,
                                 const std::shared_ptr<StandardMesh> &mesh,
                                 const std::shared_ptr<Manager::ResourceManager> &resourceManager,
                                 const std::shared_ptr<Physic::CollisionSystem3D> &collisionSystem
                                 );

        void respawn();

        void crash();

        void setPostCrashRespawnHandler(std::function<void()> handler);

        void setDirectionalLight(const std::shared_ptr<Lights::DirectionalLight> &directional_light) override;

        void setSpotLights(const std::vector<std::shared_ptr<Lights::SpotLight> > &spot_light) override;

        void setPointLights(const std::vector<std::shared_ptr<Lights::PointLight> > &point_light) override;

        void setDirection(eDIRECTION direction);

        void setBodySegment(bool bodySegment);

        void addTile(eDIRECTION direction);

        [[nodiscard]] eDIRECTION getDirection() const;

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        void renderShadows(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                           const glm::mat4 &parentTransform) const override;

        bool isReady() const;
        bool isCrashing() const;
        bool isRespawning() const;

        void setCollisionShape(const std::shared_ptr<CollisionShape::CollisionShape3D> &collisionShape);

        void computeWorldMatrix(const glm::mat4 &parentTransform) override;

        void hide();

    private:
        void stopRespawn();
        std::unique_ptr<Tools::Timer> timer;
        std::shared_ptr<SphereMesh> createTileNode() const;
        std::shared_ptr<Material::MaterialInstance> tileMaterial;
        std::shared_ptr<Feature::LightingFeature> tileLightingFeature;
        std::shared_ptr<Material::BaseMaterial> headMaterial;
        std::shared_ptr<Material::ShaderMaterial> crashMaterial;
        std::shared_ptr<Material::ShaderMaterial> respawnMaterial;
        std::shared_ptr<Material::ShaderMaterial> headRespawnMaterial;
        std::shared_ptr<Uniform::TimerUniform> timerUniform;
        std::shared_ptr<Uniform::TimerUniform> timerUniform2;
        std::shared_ptr<Physic::CollisionSystem3D> collisionSystem;
        eDIRECTION direction = NONE;
        bool respawned = false;
        bool bodySegment = false;
        bool collisionShapeVisibleBeforeCrash = true;
        bool crashedSinceLastRespawn = false;
        std::function<void()> postCrashRespawnHandler;
    };
} // Model

#endif //SNAKE3_SNAKEMESHNODE3D_H
