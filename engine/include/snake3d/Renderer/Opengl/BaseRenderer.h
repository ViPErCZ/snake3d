#ifndef SNAKE3_BASERENDERER_H
#define SNAKE3_BASERENDERER_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_precision.inl>
#include <memory>

namespace Renderer {
    class BaseRenderer;

    struct RendererEntry {
        std::shared_ptr<BaseRenderer> renderer;
        int priority;
    };

    enum MODE {
        standard = 0,
        shadowMap = 1,
        reflection = 2,
        bloom = 3
    };

    class BaseRenderer {
    public:
        explicit BaseRenderer();

        virtual ~BaseRenderer();

        virtual void render3D(float dt, uint64_t frameId) = 0;

        virtual void render2D(float dt, uint64_t frameId) {};

        virtual void renderShadowMap() = 0;

        virtual void beforeRender(MODE mode) = 0;

        virtual void afterRender() = 0;

        virtual void setShadow(bool shadow);
        virtual void resize(int width, int height, const glm::mat4 &projection) {}

        [[nodiscard]] bool isShadow() const;

        glm::vec3 compareSceneMin(glm::vec3 sceneMin);

        glm::vec3 compareSceneMax(glm::vec3 sceneMax);

    protected:
        bool shadows;
        MODE mode;
    };
} // Manager

#endif //SNAKE3_BASERENDERER_H
