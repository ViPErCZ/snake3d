#ifndef SNAKE3_COINSCENE_H
#define SNAKE3_COINSCENE_H

#include <memory>

#include "../Renderer/Opengl/Model/Game/CoinMeshNode3D.h"
#include "../Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h"
#include "../Renderer/Opengl/Scene/Scene.h"

using namespace Model;
using namespace Animations;

namespace Scenes {
    class CoinScene final : public Scene {
    public:
        CoinScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
                  const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init() override;

        [[nodiscard]] shared_ptr<CoinMeshNode3D> getCoin() const;
        [[nodiscard]] shared_ptr<CoinMeshNode3D> getRemoveCoin() const;

    protected:
        void initCoin();

        shared_ptr<CoinMeshNode3D> coin;
        shared_ptr<CoinMeshNode3D> removeCoin;
    };
} // Scenes

#endif //SNAKE3_COINSCENE_H
