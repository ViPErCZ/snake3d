#ifndef SNAKE3_ANIMRENDERER_H
#define SNAKE3_ANIMRENDERER_H

#include "BaseRenderer.h"
#include "Model/AnimationModel.h"
#include "../../Manager/ResourceManager.h"
#include "../../ItemsDto/Snake.h"
#include "../../Manager/Camera.h"

using namespace Model;
using namespace Manager;
using namespace ItemsDto;

namespace Renderer {
    class AnimRenderer final : public BaseRenderer {
    public:
        explicit AnimRenderer(shared_ptr<sSNAKE_TILE> tile, const shared_ptr<AnimationModel> &sharedPtr, Camera *camera,
            const glm::mat4 &projection, ResourceManager* resManager);
        ~AnimRenderer() override;
        void render(float dt) override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
        void addPlay(const string& name);
        void setShow(bool show);
        void setAcceleration(float acceleration) const;

    protected:
        void renderScene(const ShaderManager* shader);

        shared_ptr<AnimationModel> model;
        ResourceManager* resourceManager;
        ShaderManager* shader;
        ShaderManager* shadowShader;
        glm::mat4 projection{};
        Camera* camera;
        vector<string> animationPlay;
        shared_ptr<sSNAKE_TILE> tile;
        bool show;
    };

} // Renderer

#endif //SNAKE3_ANIMRENDERER_H
