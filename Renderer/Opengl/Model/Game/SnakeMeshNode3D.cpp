#include "SnakeMeshNode3D.h"

#include "../Standard/AnimationArrayMesh.h"
#include "../Standard/SphereMesh.h"

namespace Model {
    SnakeMeshNode3D::SnakeMeshNode3D(const shared_ptr<StandardMesh> &mesh,
                                     const shared_ptr<ResourceManager> &resourceManager) : MeshNode3D(
        mesh, resourceManager) {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        tileMaterial = make_shared<StandardMaterial>(StandardMaterial(shader, shadowsShader));
        tileMaterial->setColor({0.88, 0.05, 0.05});
        tileMaterial->setShadow(resourceManager->getTexture("depth"));
        tileMaterial->setNormalEnabled(true);
    }

    void SnakeMeshNode3D::respawn() {
        children.clear();

        transformDetached = true;
        this->x = (23 - -23) / 2 * 32 + 16;
        this->y = (-3 - -23) / 2 * 32 + 16;

        const auto sphere = createTileNode();

        const auto tile = make_shared<SnakeMeshNode3D>(sphere, resourceManager);
        tile->setPosition({-2, 0, 0});
        tile->x = x - 2;
        tile->y = y;
        addNode(tile);

        const auto tile2 = make_shared<SnakeMeshNode3D>(sphere, resourceManager);
        tile2->setPosition({-4, 0, 0});
        tile->x = x - 4;
        tile->y = y;
        addNode(tile2);

        const auto tile3 = make_shared<SnakeMeshNode3D>(sphere, resourceManager);
        tile3->setPosition({-6, 0, 0});
        tile->x = x - 6;
        tile->y = y;
        addNode(tile3);
    }

    void SnakeMeshNode3D::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
        directionalLight = directional_light;
        tileMaterial->setDirectionalLight(directional_light);
    }

    void SnakeMeshNode3D::setDirection(const eDIRECTION direction) {
        this->direction = direction;
    }

    void SnakeMeshNode3D::stop(const bool stop) const {
        try {
            dynamic_pointer_cast<AnimationArrayMesh>(mesh)->stop(stop);
        } catch (exception &e) {
            cout << "Mesh is not AnimationArrayMesh instance." << endl;
        }
    }

    shared_ptr<SphereMesh> SnakeMeshNode3D::createTileNode() const {
        auto geometry = make_shared<BaseItem>();
        geometry->setScale({0.041667f, 0.041667f, 0.041667f});
        geometry->setPosition({23, -3, -23});
        const auto sphere = make_shared<SphereMesh>(geometry, nullptr, 1.5, 0.75);
        sphere->setMaterial(tileMaterial);

        return sphere;
    }

    void SnakeMeshNode3D::addTile(const eDIRECTION direction) {
        glm::vec3 pos = {-19, 67, -23};
        const auto sphere = createTileNode();

        const auto PrevIter = children.end() - 1;

        if (getDirection() != STOP) {
            pos = (*PrevIter)->getPosition();
        } else {
            switch (direction) {
                case LEFT:
                    if ((*PrevIter)->getPosition().x - 2 >= -25) {
                        pos.x = (*PrevIter)->getPosition().x - 2;
                        pos.y = (*PrevIter)->getPosition().y;
                    }
                    break;
                case RIGHT:
                    if ((*PrevIter)->getPosition().x + 2 <= 752) {
                        pos.x = (*PrevIter)->getPosition().x + 2;
                        pos.y = (*PrevIter)->getPosition().y;
                    }
                    break;
                case UP:
                    if ((*PrevIter)->getPosition().y - 2 >= -25) {
                        pos.x = (*PrevIter)->getPosition().x;
                        pos.y = (*PrevIter)->getPosition().y - 2;
                    }
                    break;
                case DOWN:
                    if ((*PrevIter)->getPosition().y + 2 <= 752) {
                        pos.x = (*PrevIter)->getPosition().x;
                        pos.y = (*PrevIter)->getPosition().y + 2;
                    }
                    break;
                default:
                    break;
            }
        }

        const auto tile = make_shared<SnakeMeshNode3D>(sphere, resourceManager);
        tile->setPosition(pos);
        tile->x = x - 2;
        tile->y = y;

        addNode(tile);
    }

    SnakeMeshNode3D::eDIRECTION SnakeMeshNode3D::getDirection() const {
        return direction;
    }
} // Model
