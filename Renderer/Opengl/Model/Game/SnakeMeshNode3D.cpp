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

        auto geometry = make_shared<BaseItem>();
        geometry->setZoom({0.041667f, 0.041667f, 0.041667f});
        geometry->setPosition({23, -3, -23});
        const auto sphere = make_shared<SphereMesh>(geometry, nullptr, 1.5, 0.75);
        sphere->setMaterial(tileMaterial);

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

    SnakeMeshNode3D::eDIRECTION SnakeMeshNode3D::getDirection() const {
        return direction;
    }
} // Model
