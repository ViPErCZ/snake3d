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
        void addPlay(const string& name);
    protected:
        const AnimationModel* model;
        ResourceManager* resourceManager;
        ShaderManager* baseShader;
        glm::mat4 projection{};
        Camera* camera;
        vector<string> animationPlay;
    };

} // Renderer

#endif //SNAKE3_ANIMRENDERER_H
