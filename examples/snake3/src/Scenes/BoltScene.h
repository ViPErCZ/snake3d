#ifndef SNAKE3_BOLTSCENE_H
#define SNAKE3_BOLTSCENE_H

#include <memory>

#include <snake3d/Renderer/Opengl/Scene/Scene.h>
#include "../Renderer/Opengl/Model/Game/BoltLinesNode2D.h"
#include <snake3d/Renderer/Opengl/Model/Standard/2D/MeshNode2D.h>
#include <snake3d/Renderer/Opengl/Material/ShaderMaterial.h>
#include <snake3d/Tools/Timer.h>

namespace Scenes {
    class BoltScene final : public Scene {
    public:
        BoltScene(
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight>> &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight>> &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager,
            const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection,
            const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

        void init(int priority) override;
        void update() override;

    private:
        std::shared_ptr<Model::BoltLinesNode2D> boltLines;
        std::shared_ptr<Model::MeshNode2D> boltMeshNode;
        std::shared_ptr<Material::ShaderMaterial> flashMaterial;
        std::shared_ptr<Model::MeshNode2D> flashNode;

        Tools::Timer boltTimer;
        float nextBoltIn = 0.0f;
        bool boltActive = false;
        float boltElapsed = 0.0f;
        float flashAlpha = 0.0f;

        static constexpr float kBoltDuration = 0.15f;
        static constexpr float kFlashDecay = 8.0f;

        void triggerBolt();
        void generateSegments(const glm::vec3 &start, const glm::vec3 &end) const;

        static float randomNextBoltTime();
    };
} // Scenes

#endif //SNAKE3_BOLTSCENE_H
