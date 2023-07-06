#ifndef SNAKE3_ANIMRENDERER_H
#define SNAKE3_ANIMRENDERER_H

#include "BaseRenderer.h"
#include "Model/AnimationModel.h"
#include "../../Manager/ResourceManager.h"
#include "../../ItemsDto/Snake.h"
#include <chrono>
#include <functional>

using namespace Model;
using namespace Manager;
using namespace ItemsDto;

namespace Renderer {
    class AnimRenderer : public BaseRenderer {
    public:
        explicit AnimRenderer(sSNAKE_TILE* tile,
                              const Model::AnimationModel* sharedPtr, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager);
        ~AnimRenderer() override;
        void render() override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
        void addPlay(const string& name);
        void setShow(bool show);

    protected:
        const AnimationModel* model;
        ResourceManager* resourceManager;
        ShaderManager* baseShader;
        glm::mat4 projection{};
        Camera* camera;
        vector<string> animationPlay;
        sSNAKE_TILE* tile;
        bool show;
    };

} // Renderer

#endif //SNAKE3_ANIMRENDERER_H
