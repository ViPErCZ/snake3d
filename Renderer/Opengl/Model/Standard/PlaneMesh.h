#ifndef PLANEMESH_H
#define PLANEMESH_H

#include "StandardMesh.h"

namespace Model {
    class PlaneMesh final : public StandardMesh {
    public:
        explicit PlaneMesh(std::shared_ptr<Manager::ShaderProgram> baseShader, float width, float height);
    };
} // Model

#endif //PLANEMESH_H
