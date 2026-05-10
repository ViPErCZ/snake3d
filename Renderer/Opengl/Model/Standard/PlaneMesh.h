#ifndef PLANEMESH_H
#define PLANEMESH_H

#include "StandardMesh.h"

using namespace Manager;
using namespace ModelUtils;
using namespace std;

namespace Model {
    class PlaneMesh final : public StandardMesh {
    public:
        explicit PlaneMesh(shared_ptr<ShaderManager> baseShader, float width, float height);
    };
} // Model

#endif //PLANEMESH_H
