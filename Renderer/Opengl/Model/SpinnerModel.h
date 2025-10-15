#ifndef SNAKE3_SPINNER_H
#define SNAKE3_SPINNER_H

#include <memory>
#include "Standard/StandardMesh.h"
#include "Utils/Mesh.h"

using namespace ModelUtils;

namespace Model {
    class SpinnerModel final : public StandardMesh {
    public:
        explicit SpinnerModel(const shared_ptr<BaseItem> &baseItem, shared_ptr<ShaderManager> &baseShader);
        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt) const override;
        void update(float dt) override;
    protected:
        static glm::vec3 hsvToRgb(float h, float s, float v);
        std::vector<glm::vec3> colors;
        float time;
        int numInstances = 60;
        float speed = 2.0f;
        float spiralTurns = 3.0f;
    };
} // Model

#endif //SNAKE3_SPINNER_H