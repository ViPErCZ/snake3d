#ifndef SNAKE3_QUADNODE2D_H
#define SNAKE3_QUADNODE2D_H

#include <snake3d/Renderer/Opengl/Model/Standard/2D/BaseNode2D.h>

namespace Model {
    class QuadNode2D : public BaseNode2D {
        public:
            explicit QuadNode2D(float width, float height, const shared_ptr<Manager::ShaderProgram> &baseShader = nullptr);
    };
} // Model

#endif //SNAKE3_QUADNODE2D_H