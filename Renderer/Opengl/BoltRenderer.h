#ifndef SNAKE3_BOLTRENDERER_H
#define SNAKE3_BOLTRENDERER_H

#include <glm/glm.hpp>
#include <vector>

#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "BaseRenderer.h"
#include "Effects/LightningFlashEffect.h"

using namespace Manager;
using namespace Effects;

namespace Renderer {
    class BoltRenderer : public BaseRenderer {
    private:
        Camera* camera;
        glm::mat4 projection{};
        ResourceManager* resourceManager;
        ShaderManager* shader;
		unsigned int VAO{};
		unsigned int VBO{};
		float boltProgress{0.0f};
        float boltDuration{0.15f};
        LightningFlashEffect* lightning;
        
        struct BoltSegment {
            glm::vec3 start;
            glm::vec3 end;
            float thickness;
            float brightness;
        };
        
        std::vector<BoltSegment> segments;
        float timeSinceLastBolt;
        float nextBoltTime;
        bool isActive;
        
        void generateBoltSegments(const glm::vec3& start, const glm::vec3& end);
        void createBoltGeometry();
		void cleanup();
        
    public:
        BoltRenderer(Camera* camera, glm::mat4 proj, ResourceManager* resManager);
        ~BoltRenderer() override;
        
        void render3D(float dt) override;
        void beforeRender() override;
        void afterRender() override;
        void renderShadowMap() override;
        void triggerBolt(); // Manuální spuštění blesku
    };
}

#endif //SNAKE3_BOLTRENDERER_H