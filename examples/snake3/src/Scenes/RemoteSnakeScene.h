#ifndef SNAKE3_REMOTESNAKESCENE_H
#define SNAKE3_REMOTESNAKESCENE_H

#include <memory>
#include <vector>

#include "../Handler/SnakeMoveHandler.h"
#include "Physic/Dynamics/DynamicBody.h"
#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"
#include "Renderer/Opengl/Scene/Scene.h"

namespace Scenes {
    class RemoteSnakeScene final : public Scene {
    public:
        RemoteSnakeScene(
            const std::shared_ptr<DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<PointLight> > &pointLights,
            const std::shared_ptr<RenderManager> &rendererManager, const std::shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const std::shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        [[nodiscard]] std::shared_ptr<Model::SnakeMeshNode3D> getSnake() const;
        [[nodiscard]] std::shared_ptr<Handler::SnakeMoveHandler> getMoveHandler() const;

        void setServerControlled(bool enabled) const;
        void setActive(bool active) const;
        void setSpawnLayout(const glm::vec3 &headPosition, Model::SnakeMeshNode3D::eDIRECTION direction) const;
        [[nodiscard]] std::vector<glm::vec3> collectPositions() const;
        void applyNetworkInput(int moveX, int moveY, uint8_t actions) const;
        void updateAuthoritative() const;
        void respawnAt(const glm::vec3 &headPosition, Model::SnakeMeshNode3D::eDIRECTION direction) const;

    private:
        static int toVirtualCoord(float worldCoord);
        void initSnake();
        void initMoveHandler();
        void ensureLength(size_t segmentCount) const;
        void configureCollisionLayers() const;
        static void applyDirectionVisual(const std::shared_ptr<Model::SnakeMeshNode3D> &node, Model::SnakeMeshNode3D::eDIRECTION direction);
        static void setVisibleRecursive(const std::shared_ptr<Model::MeshNode3D> &node, bool visible);
        static void setCollisionEnabledRecursive(const std::shared_ptr<Model::MeshNode3D> &node, bool enabled);

        std::shared_ptr<Model::SnakeMeshNode3D> snake;
        std::shared_ptr<Handler::SnakeMoveHandler> snakeMoveHandler;
        std::shared_ptr<Physic::Dynamics::DynamicBody> snakeBody;
    };
} // Scenes

#endif // SNAKE3_REMOTESNAKESCENE_H
