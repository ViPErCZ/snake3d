#ifndef SNAKE3_BASERENDERER_H
#define SNAKE3_BASERENDERER_H

#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.inl>

namespace Renderer {
    class BaseRenderer {
    public:
        explicit BaseRenderer();

        virtual ~BaseRenderer();

        virtual void render3D(float dt) = 0;

        virtual void render2D(float dt) {};

        virtual void renderShadowMap() = 0;

        virtual void beforeRender() = 0;

        virtual void afterRender() = 0;

        virtual void setShadow(bool shadow);

        [[nodiscard]] bool isShadow() const;

        void setFog(bool fog);

        [[nodiscard]] bool isFog() const;

        glm::vec3 compareSceneMin(glm::vec3 sceneMin);

        glm::vec3 compareSceneMax(glm::vec3 sceneMax);

    protected:
        bool shadows;
        bool fog;
    };
} // Manager

#endif //SNAKE3_BASERENDERER_H
