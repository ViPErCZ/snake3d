#ifndef SNAKE3_BOLTLINESNODE2D_H
#define SNAKE3_BOLTLINESNODE2D_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include <snake3d/Renderer/Opengl/Model/Standard/2D/BaseNode2D.h>

namespace Model {
    class BoltLinesNode2D : public BaseNode2D {
    public:
        struct Segment {
            glm::vec3 start;
            glm::vec3 end;
            float brightness;
        };

        BoltLinesNode2D(const glm::mat4 &perspectiveProjection, const std::shared_ptr<Manager::ShaderProgram> &shader);
        ~BoltLinesNode2D() override;

        void setSegments(const std::vector<Segment> &segs);
        void clearSegments();

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const override;

    private:
        glm::mat4 perspectiveProj;
        std::vector<Segment> segments;
        GLuint linesVAO = 0;
        GLuint linesVBO = 0;

        void initGeometry();
    };
} // Model

#endif //SNAKE3_BOLTLINESNODE2D_H
