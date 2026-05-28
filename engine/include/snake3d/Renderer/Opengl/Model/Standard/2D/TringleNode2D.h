#ifndef SNAKE3_TRINGLENODE2D_H
#define SNAKE3_TRINGLENODE2D_H

#include <snake3d/Renderer/Opengl/Model/Standard/2D/BaseNode2D.h>

namespace Model {
    class TringleNode2D : public BaseNode2D {
        public:
            explicit TringleNode2D(float width, float height, const shared_ptr<Manager::ShaderProgram> &baseShader = nullptr);
    };
} // Model

#endif //SNAKE3_TRINGLENODE2D_H