#ifndef SNAKE3_SPINNER_H
#define SNAKE3_SPINNER_H

#include <memory>
#include "Standard/StandardMesh.h"
#include "Standard/TringleMesh3D.h"

namespace Model {
    class SpinnerMesh final : public TringleMesh3D {
    public:
        explicit SpinnerMesh(const shared_ptr<ShaderProgram> &baseShader);
        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                            const glm::mat4 &parentTransform, bool shadows) const override;
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