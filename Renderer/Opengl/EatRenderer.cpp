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
            const glm::vec4 * rotate = eat->getRotate();
            glTranslatef( position.x, position.y, position.z );
            glScalef(zoom.x, zoom.y, zoom.z);

            Uint32 now = SDL_GetTicks();
            float angle = rotate[1].x;
            if (now > lastTime + 20) {
                angle++;
                lastTime = now;
            }
            glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
            glRotatef(angle, rotate[1].y, rotate[1].z, rotate[1].w);
            glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);
            eat->setRotate(rotate[0], {angle, 0, 1, 0}, rotate[2]);

//            // normals
//            glColor3f(0,0,1);
//            glBegin(GL_LINES);
//            for (unsigned short indice : indices){
//                glm::vec3 p = indexed_vertices[indice];
//                glVertex3fv(&p.x);
//                glm::vec3 o = glm::normalize(indexed_normals[indice]);
//                p+=o*0.1f;
//                glVertex3fv(&p.x);
//            }
//            glEnd();
//            // tangents
//            glColor3f(1,0,0);
//            glBegin(GL_LINES);
//            for (unsigned short indice : indices){
//                glm::vec3 p = indexed_vertices[indice];
//                glVertex3fv(&p.x);
//                glm::vec3 o = glm::normalize(indexed_tangents[indice]);
//                p+=o*0.1f;
//                glVertex3fv(&p.x);
//            }
//            glEnd();
//            // bitangents
//            glColor3f(0,1,0);
//            glBegin(GL_LINES);
//            for (unsigned short indice : indices){
//                glm::vec3 p = indexed_vertices[indice];
//                glVertex3fv(&p.x);
//                glm::vec3 o = glm::normalize(indexed_bitangents[indice]);
//                p+=o*0.1f;
//                glVertex3fv(&p.x);
//            }
//            glEnd();*/

//            glBegin(GL_TRIANGLES);
//            int x = 0;
//            for (auto Iter = eat->getVertices().begin(); Iter < eat->getVertices().end(); Iter++) {
//                glVertex3f( (*Iter).x, (*Iter).y,  (*Iter).z); glTexCoord2f( eat->getUvs()[x].x, eat->getUvs()[x].y );
//            }
//            glEnd( );

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, eat->getVertexBuffer());
            glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    nullptr           // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, eat->getUvBuffer());
            glVertexAttribPointer(
                    1,                                // attribute
                    2,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    nullptr                      // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, eat->getNormalBuffer());
            glVertexAttribPointer(
                    2,                                // attribute
                    3,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    nullptr                          // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eat->getElementBuffer());

            // Draw the triangles !
            glDrawElements(
                    GL_TRIANGLES,      // mode
                    (GLsizei)eat->getIndices().size(),    // count
                    GL_UNSIGNED_SHORT, // type
                    nullptr           // element array buffer offset
            );

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            // enable vertex coordinates
            glDisableClientState(GL_VERTEX_ARRAY);
            // enable textures
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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