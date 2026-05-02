#ifndef SNAKE3_REMOTESNAKESCENE_H
#define SNAKE3_REMOTESNAKESCENE_H

#include <memory>
#include <vector>

#include "../Handler/SnakeMoveHandler.h"
#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"
#include "../Renderer/Opengl/Scene/Scene.h"

using namespace Model;

namespace Scenes {
    class RemoteSnakeScene final : public Scene {
    public:
        RemoteSnakeScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;

        [[nodiscard]] shared_ptr<SnakeMeshNode3D> getSnake() const;
        [[nodiscard]] shared_ptr<SnakeMoveHandler> getMoveHandler() const;

        void setServerControlled(bool enabled) const;
        void setActive(bool active) const;
        void setSpawnLayout(const glm::vec3 &headPosition, SnakeMeshNode3D::eDIRECTION direction) const;
        [[nodiscard]] std::vector<glm::vec2> collectPositions() const;
        void applyNetworkInput(int moveX, int moveY, uint8_t actions) const;
        void updateAuthoritative() const;
        void respawnAt(const glm::vec3 &headPosition, SnakeMeshNode3D::eDIRECTION direction) const;

    private:
        static int toVirtualCoord(float worldCoord);
        void initSnake();
        void initMoveHandler();
        void ensureLength(size_t segmentCount) const;
        void configureCollisionLayers() const;
        static void applyDirectionVisual(const shared_ptr<SnakeMeshNode3D> &node, SnakeMeshNode3D::eDIRECTION direction);
        static void setVisibleRecursive(const shared_ptr<MeshNode3D> &node, bool visible);
        static void setCollisionEnabledRecursive(const shared_ptr<MeshNode3D> &node, bool enabled);

        shared_ptr<SnakeMeshNode3D> snake;
        shared_ptr<SnakeMoveHandler> snakeMoveHandler;
    };
} // Scenes

#endif // SNAKE3_REMOTESNAKESCENE_H
