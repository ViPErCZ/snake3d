#ifndef SNAKE3_OBJWALLRENDERER_H
#define SNAKE3_OBJWALLRENDERER_H

#include "../../Manager/ShaderManager.h"
#include "../../Manager/ResourceManager.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../ItemsDto/ObjWall.h"
#include "../../ItemsDto/Snake.h"
#include "../../Manager/Camera.h"

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

    class ObjWallRenderer : public BaseRenderer {
    public:
        explicit ObjWallRenderer(const shared_ptr<Snake> &snake, const shared_ptr<ObjWall> &item, Camera* camera, const glm::mat4 &proj, ResourceManager* resManager);
        ~ObjWallRenderer() override;
        void render3D(float dt) override;
        void beforeRender() override;
        void afterRender() override;
        void renderShadowMap() override;
        void toggleParallax();
        void downScale();
        void upScale();

    protected:
        static bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& boxMin, const glm::vec3& boxMax, float maxDistance);
        shared_ptr<Snake> snake;
        shared_ptr<ObjWall> wall;
        Mesh* mesh;
        Camera* camera;
        glm::mat4 projection{};
        ResourceManager* resourceManager;
        ShaderManager* shader;
        TextureManager* texture1;
        TextureManager* texture2;
        TextureManager* texture3;
        TextureManager* texture4;
        bool parallax;
    };

} // Renderer

#endif //SNAKE3_OBJWALLRENDERER_H
