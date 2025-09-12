#ifndef SNAKE3_STANDARDMESHRENDERER_H
#define SNAKE3_STANDARDMESHRENDERER_H

#include <memory>

#include "BaseRenderer.h"
#include "Model/Standard/StandardMesh.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/Camera.h"

using namespace Model;
using namespace std;

namespace Renderer {
    class StandardMeshRenderer final : public BaseRenderer {
    public:
        explicit StandardMeshRenderer(shared_ptr<Camera> camera,
                                      const glm::mat4 &projection,
                                      shared_ptr<StandardMesh> standardMesh);

        ~StandardMeshRenderer() override;

        void render(float dt) override;

        void beforeRender() override;

        void afterRender() override;

        void renderShadowMap() override;

    protected:
        void renderScene(const shared_ptr<ShaderManager> &shader) const;
        shared_ptr<Mesh> getMesh();
        shared_ptr<Camera> camera;
        shared_ptr<StandardMesh> mesh;
        glm::mat4 projection;
    };
} // Renderer

#endif //SNAKE3_STANDARDMESHRENDERER_H
