#include "EatRemoveAnimateRenderer.h"

namespace Renderer {
    EatRemoveAnimateRenderer::EatRemoveAnimateRenderer(Eat *eat): eat(eat) {}

    EatRemoveAnimateRenderer::~EatRemoveAnimateRenderer() {
        delete eat;
    }

    void EatRemoveAnimateRenderer::render() {

        if (eat && !completed && eat->isVisible()) {
            GLuint index = eat->getPrimaryTexture();
            if (index > 0) {
                glBindTexture(GL_TEXTURE_2D, index);
            }
            glLoadIdentity();

            glm::vec3 position = eat->getPosition();
            glm::vec3 zoom = eat->getZoom();
            const glm::vec4 *rotate = eat->getRotate();
            glTranslatef(position.x, position.y, position.z);
            glScalef(zoom.x, zoom.y, zoom.z);

            double now = glfwGetTime();
            float angle = rotate[1].x;
            if (now > lastTime + 8) {
                angle++;
                zoom.x -= 0.1;
                zoom.y -= 0.1;
                zoom.z -= 0.1;

                position.z += 0.9;

                eat->setPosition(position);
                eat->setZoom(zoom);

                lastTime = now;
            }
            glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
            glRotatef(angle, rotate[1].y, rotate[1].z, rotate[1].w);
            glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);
            eat->setRotate(rotate[0], {angle, 0, 1, 0}, rotate[2]);

//            // 1rst attribute buffer : vertices
//            glEnableVertexAttribArray(0);
//            glBindBuffer(GL_ARRAY_BUFFER, eat->getVertexBuffer());
//            glVertexAttribPointer(
//                    0,                  // attribute
//                    3,                  // size
//                    GL_FLOAT,           // type
//                    GL_FALSE,           // normalized?
//                    0,                  // stride
//                    nullptr           // array buffer offset
//            );
//
//            // 2nd attribute buffer : UVs
//            glEnableVertexAttribArray(1);
//            glBindBuffer(GL_ARRAY_BUFFER, eat->getUvBuffer());
//            glVertexAttribPointer(
//                    1,                                // attribute
//                    2,                                // size
//                    GL_FLOAT,                         // type
//                    GL_FALSE,                         // normalized?
//                    0,                                // stride
//                    nullptr                      // array buffer offset
//            );
//
//            // 3rd attribute buffer : normals
//            glEnableVertexAttribArray(2);
//            glBindBuffer(GL_ARRAY_BUFFER, eat->getNormalBuffer());
//            glVertexAttribPointer(
//                    2,                                // attribute
//                    3,                                // size
//                    GL_FLOAT,                         // type
//                    GL_FALSE,                         // normalized?
//                    0,                                // stride
//                    nullptr                          // array buffer offset
//            );
//
//            // Index buffer
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eat->getElementBuffer());
//
//            // Draw the triangles !
//            glDrawElements(
//                    GL_TRIANGLES,      // mode
//                    (GLsizei)eat->getIndices().size(),    // count
//                    GL_UNSIGNED_SHORT, // type
//                    nullptr           // element array buffer offset
//            );
//
//            glDisableVertexAttribArray(0);
//            glDisableVertexAttribArray(1);
//            glDisableVertexAttribArray(2);
//            // enable vertex coordinates
//            glDisableClientState(GL_VERTEX_ARRAY);
//            // enable textures
//            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

            if (zoom.x <= 0) {
                completed = true;
            }
        }
    }

    void EatRemoveAnimateRenderer::beforeRender() {
//        glEnable(GL_LIGHTING);
//        glEnable(GL_LIGHT0);
        glShadeModel(GL_SMOOTH);
    }

    void EatRemoveAnimateRenderer::afterRender() {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

    bool EatRemoveAnimateRenderer::isCompleted() const {
        return completed;
    }

    void EatRemoveAnimateRenderer::setCompleted(bool completed) {
        EatRemoveAnimateRenderer::completed = completed;
    }

} // Renderer