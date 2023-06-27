#ifndef SNAKE3_ANIMRENDERER_H
#define SNAKE3_ANIMRENDERER_H

#include "BaseRenderer.h"
#include "Model/AnimationModel.h"
#include "../../Manager/ResourceManager.h"
#include <chrono>
#include <functional>

using namespace Model;
using namespace Manager;

namespace Renderer {

    class AnimRenderer : public BaseRenderer {
    public:
        explicit AnimRenderer(const Model::AnimationModel* sharedPtr, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager);

        void render() override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        [[nodiscard]] const AnimationNode* findAnimationNode(const Bone& bone) const noexcept;
        const AnimationModel* model;
        ResourceManager* resourceManager;
        ShaderManager* baseShader;
        glm::mat4 projection;
        Camera* camera;
        const Animation* animation {};
        chrono::time_point<std::chrono::steady_clock> last_time;
        chrono::duration<double> animation_duration;
        vector<glm::mat4> bone_transform;
    };

} // Renderer

#endif //SNAKE3_ANIMRENDERER_H
