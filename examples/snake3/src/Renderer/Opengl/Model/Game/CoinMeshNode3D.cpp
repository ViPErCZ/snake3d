#include "CoinMeshNode3D.h"

using namespace std;
using namespace Manager;
using namespace Tools;
using namespace Lights;

namespace Model {
    CoinMeshNode3D::CoinMeshNode3D(const shared_ptr<SpotLight> &spotLight, const shared_ptr<ContextState> &contextState,
                                   const shared_ptr<StandardMesh> &mesh,
                                   const shared_ptr<ResourceManager> &resourceManager)
        : MeshNode3D(contextState, mesh, resourceManager), spotLight(spotLight) {
    }

    void CoinMeshNode3D::update(const float dt, const uint64_t frameId) {
        // update spotLight
        if (spotLight) {
            spotLight->setVisible(isVisible());
            const auto coinPos = getPosition();
            if (coinPos == lastPos) {
                return;
            }
            const auto coinScale = getScale();
            const float realX = coinPos.x * coinScale.x;
            const float realY = coinPos.y * coinScale.y;
            const glm::vec3 spotPos = {realX, realY, -1.5f};
            const glm::vec3 spotDir = {spotPos.x, spotPos.y, 0.0f};
            spotLight->setPosition(spotPos);
            spotLight->setDirection(spotDir);
            lastPos = coinPos;
        }

        MeshNode3D::update(dt, frameId);
    }
} // Model
