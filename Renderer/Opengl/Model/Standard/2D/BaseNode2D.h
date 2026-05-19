#ifndef SNAKE3_BASENODE2D_H
#define SNAKE3_BASENODE2D_H

#include <memory>

#include "../../../../../Manager/Camera.h"
#include "../../../../../Manager/ShaderProgram.h"
#include "../../../Material/BaseMaterial.h"
#include "../../Utils/Mesh2D.h"

using namespace std;
using namespace ModelUtils;
using namespace Manager;
using namespace Material;

namespace Model {
    class BaseNode2D {
    public:
        explicit BaseNode2D(const shared_ptr<ShaderProgram> &baseShader);

        virtual ~BaseNode2D() = default;

        void setMaterial(const shared_ptr<BaseMaterial> &material);

        void setColor(const glm::vec3 &color);

        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                            const glm::mat4 &parentTransform) const;

        [[nodiscard]] shared_ptr<Mesh2D> getMesh() const;

        virtual void update(float dt) {};

        void setBlending(Blending blending);

        void setDepthTest(bool depthTest);

        void setDepthWrite(bool depthWrite);

        [[nodiscard]] Blending getBlending() const;

        [[nodiscard]] bool getDepthTest() const;

        [[nodiscard]] bool getDepthWrite() const;

        void bind() const;

        [[nodiscard]] unsigned long indicesCount() const;

    protected:
        shared_ptr<Mesh2D> mesh;
        shared_ptr<BaseMaterial> material;
        shared_ptr<ShaderProgram> baseShader;
        unsigned int textureId = 0;
        glm::vec3 color;
        Blending blending = Blending::Opaque;
        bool depthTest = true;
        bool depthWrite = true;
    };
} // Model

#endif //SNAKE3_BASENODE2D_H
