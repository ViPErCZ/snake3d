#include "SnakeMeshNode3D.h"

#include "../Standard/SphereMesh.h"

namespace Model {
    SnakeMeshNode3D::SnakeMeshNode3D(const shared_ptr<StandardMesh> &mesh, const shared_ptr<ResourceManager> &resourceManager) :
        MeshNode3D(mesh, resourceManager) {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        tileMaterial = make_shared<StandardMaterial>(StandardMaterial(shader, shadowsShader));
        tileMaterial->setColor({0.88, 0.05, 0.05});
        tileMaterial->setShadow(resourceManager->getTexture("depth"));
        tileMaterial->setNormalEnabled(true);
    }

    void SnakeMeshNode3D::respawn() {
        children.clear();

        auto geometry = make_shared<BaseItem>();
        geometry->setZoom({0.041667f, 0.041667f, 0.041667f});
        geometry->setPosition( {23, -3, -23});
        const auto sphere = make_shared<SphereMesh>(geometry, nullptr, 1.5, 0.75);
        sphere->setMaterial(tileMaterial);

        const auto tile = make_shared<MeshNode3D>(sphere, resourceManager);
        tile->setPosition({-2, 0, 0});
        addNode(tile);

        const auto tile2 = make_shared<MeshNode3D>(sphere, resourceManager);
        tile2->setPosition({-4, 0, 0});
        addNode(tile2);

        const auto tile3 = make_shared<MeshNode3D>(sphere, resourceManager);
        tile3->setPosition({-6, 0, 0});
        addNode(tile3);
    }

    void SnakeMeshNode3D::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
        directionalLight = directional_light;
        tileMaterial->setDirectionalLight(directional_light);

    }
} // Model