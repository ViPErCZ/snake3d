#ifndef SNAKE3_BASERENDERER_H
#define SNAKE3_BASERENDERER_H

#include <memory>
#include "../../ItemsDto/BaseItem.h"
#include "Model/Utils/Mesh.h"

using namespace ItemsDto;
using namespace ModelUtils;

namespace Renderer {
    class BaseRenderer {
    public:
        BaseRenderer();

        explicit BaseRenderer(BaseItem *item);

        virtual ~BaseRenderer();

        virtual void render(float dt) = 0;

        virtual void renderShadowMap() = 0;

        virtual void beforeRender() = 0;

        virtual void afterRender() = 0;

        virtual void setShadow(bool shadow);

        [[nodiscard]] bool isShadow() const;

        void setFog(bool fog);

        [[nodiscard]] bool isFog() const;

        [[nodiscard]] virtual bool isPlane();

        glm::vec3 compareSceneMin(glm::vec3 sceneMin);

        glm::vec3 compareSceneMax(glm::vec3 sceneMax);

    protected:
        virtual shared_ptr<Mesh> getMesh();
        shared_ptr<BaseItem> item{};
        bool shadows;
        bool fog;
    };
} // Manager

#endif //SNAKE3_BASERENDERER_H
