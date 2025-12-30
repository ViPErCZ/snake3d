#ifndef SNAKE3_BASERENDERER_H
#define SNAKE3_BASERENDERER_H

#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.inl>
#include <memory>

namespace Renderer {
    class BaseRenderer;

    struct RendererEntry {
        std::shared_ptr<BaseRenderer> renderer;
        int priority;
    };

    class BaseRenderer {
    public:
        explicit BaseRenderer();

        virtual ~BaseRenderer();

        virtual void render3D(float dt, uint64_t frameId) = 0;

        virtual void render2D(float dt, uint64_t frameId) {};

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
