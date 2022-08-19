#include "EatRenderer.h"

namespace Renderer {
    EatRenderer::EatRenderer(Eat *eat) : eat(eat) {}

    void EatRenderer::render() {
        if (eat->isVisible()) {
            // enable vertex coordinates
//            glEnableClientState(GL_VERTEX_ARRAY);
//            // enable textures
//            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            GLuint index = eat->getPrimaryTexture();
            glBindTexture(GL_TEXTURE_2D, index);

            glLoadIdentity();

            glm::vec3 position = eat->getPosition();
            glm::vec3 zoom = eat->getZoom();
            const glm::vec4 *rotate = eat->getRotate();
            glTranslatef(position.x, position.y, position.z);
            glScalef(zoom.x, zoom.y, zoom.z);

            double now = glfwGetTime();
            float angle = rotate[1].x;
            if (now > lastTime + 20) {
                angle++;
                lastTime = now;
            }
            glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
            glRotatef(angle, rotate[1].y, rotate[1].z, rotate[1].w);
            glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);
            eat->setRotate(rotate[0], {angle, 0, 1, 0}, rotate[2]);
        }

        glActiveTexture(GL_TEXTURE0);
    }

    void EatRenderer::beforeRender() {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
//        float LightPos2[4] = {0.5f, 1.0f, 0.4f, 0.0f};
//        float Ambient[4] = {1.0f, 0.0f, 0.0f, 1.0f};
//        glLightfv(GL_LIGHT0, GL_POSITION, LightPos2);
//        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
        glShadeModel(GL_SMOOTH);
    }

    void EatRenderer::afterRender() {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glUseProgram(0);
    }
} // Renderer