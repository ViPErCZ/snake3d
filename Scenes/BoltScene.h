#ifndef SNAKE3_BOLTSCENE_H
#define SNAKE3_BOLTSCENE_H

#include <memory>

#include "../Renderer/Opengl/Scene/Scene.h"
#include "../Renderer/Opengl/Model/Game/BoltLinesNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/MeshNode2D.h"
#include "../Renderer/Opengl/Material/ShaderMaterial.h"
#include "../Tools/Timer.h"

using namespace std;
using namespace Model;
using namespace Material;
using namespace Tools;

namespace Scenes {
    class BoltScene final : public Scene {
    public:
        BoltScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight>> &spotLights,
            const vector<shared_ptr<PointLight>> &pointLights,
            const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;
        void update() override;

    private:
        shared_ptr<BoltLinesNode2D> boltLines;
        shared_ptr<MeshNode2D> boltMeshNode;
        shared_ptr<ShaderMaterial> flashMaterial;
        shared_ptr<MeshNode2D> flashNode;

        Timer boltTimer;
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
