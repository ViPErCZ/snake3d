#ifndef SNAKE3_BASENODE2D_H
#define SNAKE3_BASENODE2D_H

#include <memory>
#include "../../../../../Manager/ShaderManager.h"
#include "../../Utils/Mesh.h"

using namespace std;
using namespace ModelUtils;
using namespace Manager;

namespace Model {
    class BaseNode2D {
    public:
        explicit BaseNode2D(const shared_ptr<ShaderManager> &baseShader);

        virtual ~BaseNode2D() = default;

        [[nodiscard]] shared_ptr<Mesh> getMesh() const;

    protected:
        shared_ptr<Mesh> mesh;
        shared_ptr<ShaderManager> baseShader;
    };
} // Model

#endif //SNAKE3_BASENODE2D_H
