#ifndef SNAKE3_BASENODE2D_H
#define SNAKE3_BASENODE2D_H

#include <memory>

#include <snake3d/Manager/Camera.h>
#include <snake3d/Manager/ShaderProgram.h>
#include <snake3d/Renderer/Opengl/Material/BaseMaterial.h>
#include <snake3d/Renderer/Opengl/Model/Utils/Mesh2D.h>

namespace Model {
    using std::shared_ptr;

    class BaseNode2D {
    public:
        explicit BaseNode2D(const shared_ptr<Manager::ShaderProgram> &baseShader);

        virtual ~BaseNode2D() = default;

        void setMaterial(const shared_ptr<Material::BaseMaterial> &material);

        void setColor(const glm::vec3 &color);

        virtual void render(const shared_ptr<Manager::Camera> &camera, const glm::mat4 &ortho, float dt,
                            const glm::mat4 &parentTransform) const;

        [[nodiscard]] shared_ptr<ModelUtils::Mesh2D> getMesh() const;

        virtual void update(float dt) {};

        void setBlending(Tools::Blending blending);

        void setDepthTest(bool depthTest);

        void setDepthWrite(bool depthWrite);

        [[nodiscard]] Tools::Blending getBlending() const;

        [[nodiscard]] bool getDepthTest() const;

        [[nodiscard]] bool getDepthWrite() const;

        void bind() const;

        [[nodiscard]] unsigned long indicesCount() const;

    protected:
        shared_ptr<ModelUtils::Mesh2D> mesh;
        shared_ptr<Material::BaseMaterial> material;
        shared_ptr<Manager::ShaderProgram> baseShader;
        unsigned int textureId = 0;
        glm::vec3 color;
        Tools::Blending blending = Tools::Blending::Opaque;
        bool depthTest = true;
        bool depthWrite = true;
    };
} // Model

#endif //SNAKE3_BASENODE2D_H
