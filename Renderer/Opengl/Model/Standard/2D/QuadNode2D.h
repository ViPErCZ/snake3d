#ifndef SNAKE3_QUADNODE2D_H
#define SNAKE3_QUADNODE2D_H

#include "BaseNode2D.h"

namespace Model {
    class QuadNode2D : public BaseNode2D {
        public:
            explicit QuadNode2D(float width, float height, const shared_ptr<ShaderManager> &baseShader = nullptr);
    };
} // Model

#endif //SNAKE3_QUADNODE2D_H