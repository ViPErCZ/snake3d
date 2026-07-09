#include "Renderer/Opengl/Model/Game/LitMeshNode3D.h"

using namespace std;
using namespace Lights;

namespace Model {
    void LitMeshNode3D::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
        directionalLight = directional_light;
    }

    void LitMeshNode3D::setSpotLights(const vector<shared_ptr<SpotLight>> &spot_light) {
        spotLights = spot_light;
    }

    void LitMeshNode3D::setPointLights(const vector<shared_ptr<PointLight>> &point_light) {
        pointLights = point_light;
    }
} // Model
