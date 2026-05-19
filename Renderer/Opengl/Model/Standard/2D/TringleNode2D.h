#ifndef SNAKE3_TRINGLENODE2D_H
#define SNAKE3_TRINGLENODE2D_H

#include "BaseNode2D.h"

namespace Model {
    class TringleNode2D : public BaseNode2D {
        public:
            explicit TringleNode2D(float width, float height, const shared_ptr<ShaderProgram> &baseShader = nullptr);
    };
} // Model

#endif //SNAKE3_TRINGLENODE2D_H