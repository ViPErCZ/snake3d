#include "ObjWallRenderer.h"

namespace Renderer {
    ObjWallRenderer::ObjWallRenderer(ObjWall *item) {
        wall = item;
        shaderManager = new ShaderManager();
        shaderManager->loadShader("Assets/Shaders/normal_map.vs", "Assets/Shaders/normal_map.fs");
    }

    ObjWallRenderer::~ObjWallRenderer() {
        delete wall;
        delete shaderManager;
    }

    void ObjWallRenderer::render() {
//        shaderManager->use();
        for (auto Iter = wall->getItems().begin(); Iter < wall->getItems().end(); Iter++) {
            if ((*Iter)->isVisible()) {
                glm::vec3 position = (*Iter)->getPosition();
                if ((*Iter)->isBlend()) {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
                }

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 4); // 2
                //shaderManager->setInt("diffuseMap", 0);
//                glActiveTexture(GL_TEXTURE1);
//                glBindTexture(GL_TEXTURE_2D, 1);
                //shaderManager->setInt("normalMap", 1);

                glLoadIdentity();

                glm::vec3 zoom = (*Iter)->getZoom();
                const glm::vec4 *rotate = (*Iter)->getRotate();
                glTranslatef(position.x, position.y, position.z);
                glScalef(zoom.x, zoom.y, zoom.z);
                glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
                glRotatef(rotate[1].x, rotate[1].y, rotate[1].z, rotate[1].w);
                glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);

                // 1rst attribute buffer : vertices
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, (*Iter)->getVertexBuffer());
                glVertexAttribPointer(
                        0,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        nullptr           // array buffer offset
                );

                // 2nd attribute buffer : UVs
                glEnableVertexAttribArray(2);
                glBindBuffer(GL_ARRAY_BUFFER, (*Iter)->getUvBuffer());
                glVertexAttribPointer(
                        2,                                // attribute
                        2,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        nullptr                      // array buffer offset
                );

                // 3rd attribute buffer : normals
                glEnableVertexAttribArray(3);
                glBindBuffer(GL_ARRAY_BUFFER, (*Iter)->getNormalBuffer());
                glVertexAttribPointer(
                        3,                                // attribute
                        3,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        nullptr                          // array buffer offset
                );

                // Index buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*Iter)->getElementBuffer());

                // Draw the triangles !
                glDrawElements(
                        GL_TRIANGLES,      // mode
                        (GLsizei) (*Iter)->getIndices().size(),    // count
                        GL_UNSIGNED_SHORT, // type
                        nullptr           // element array buffer offset
                );

                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
                glDisableVertexAttribArray(2);
            }
        }
    }

    void ObjWallRenderer::beforeRender() {
        glEnable(GL_TEXTURE_2D);
//        glEnable(GL_LIGHTING);
//        glEnable(GL_LIGHT0);
//        float LightPos2[4] = {0.5f, 1.0f, 0.4f, 0.0f};
//        float Ambient[4] = {1.0f, 0.0f, 0.0f, 1.0f};
//        glLightfv(GL_LIGHT0, GL_POSITION, LightPos2);
//        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
    }

    void ObjWallRenderer::afterRender() {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_BLEND);
        glUseProgram(0);
    }
} // Renderer