#include "WallRenderer.h"

namespace Renderer {
    Renderer::WallRenderer::WallRenderer(Wall *item) {
        wall = item;
        shaderManager = new ShaderManager();
//        shaderManager->loadShader("Assets/Shaders/normal_map.vs", "Assets/Shaders/normal_map.fs");
    }

    Renderer::WallRenderer::~WallRenderer() {
        delete wall;
        delete shaderManager;
    }

    void Renderer::WallRenderer::render() {
//        glm::mat4 projection = glm::perspective(glm::radians(40.0), (double)1920/(double)1080, 1.5, 2600.0);
//        projection = glm::translate(projection, {54, -247, -450});
//        projection = glm::rotate(projection, glm::radians(-60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//        projection = glm::rotate(projection, glm::radians(41.0f), glm::normalize(glm::vec3(0.0, 1.0, 1.0)));
//
//        glm::vec3 cameraPos = {214.0f, 67.0f, 0.0f};
//        glm::vec3 frontPos = {214.0f, 67.0f, -1.0f};
//        glm::mat4 view = glm::lookAt(glm::normalize(cameraPos), glm::normalize(frontPos + cameraPos), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
////        shaderManager->use();
//        shaderManager->setMat4("projection", projection);
//        shaderManager->setMat4("view", view);
//        shaderManager->setVec3("viewPos", cameraPos);
//        glm::vec3 lightPos(0.5f, 1.0f, 0.2f);
//        shaderManager->setVec3("lightPos", lightPos);

        for (auto Iter = wall->getItems().begin(); Iter < wall->getItems().end(); Iter++) {
            if ((*Iter)->isVisible()) {
                glm::vec3 position = (*Iter)->getPosition();

                if ((*Iter)->isBlend()) {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
                }

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 2); // 2
                //shaderManager->setInt("diffuseMap", 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, 4);
                //shaderManager->setInt("normalMap", 1);

                glLoadIdentity();

                glm::vec3 zoom = (*Iter)->getZoom();
                const glm::vec4 *rotate = (*Iter)->getRotate();
                glTranslatef(position.x, position.y, position.z);
                glScalef(zoom.x, zoom.y, zoom.z);
                glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
                glRotatef(rotate[1].x, rotate[1].y, rotate[1].z, rotate[1].w);
                glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);

                glBegin(GL_QUADS);

                // Přední stěna
                glNormal3f(0.0f, 0.0f, 1.0f);// Normála
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, 1.0f, 1.0f);

                // Zadní stěna
                glNormal3f(0.0f, 0.0f, -1.0f);// Normála
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, -1.0f, 1.0f);

                // Horní stěna
                glNormal3f(0.0f, 1.0f, 0.0f);// Normála
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(-1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, -1.0f, 1.0f);

                // Spodní stěna
                glNormal3f(0.0f, -1.0f, 0.0f);// Normála
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(-1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, 1.0f, 1.0f);

                // Pravá stěna
                glNormal3f(1.0f, 0.0f, 0.0f);// Normála
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, 1.0f, 1.0f);

                // Levá stěna
                glNormal3f(-1.0f, 0.0f, 0.0f);// Normála
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, -1.0f, 1.0f);

                glEnd();

                if ((*Iter)->isBlend()) {
                    glDisable(GL_BLEND);
                }
            }
        }

        glActiveTexture(GL_TEXTURE0);
    }

    void Renderer::WallRenderer::beforeRender() {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
//        float LightPos2[4] = {0.5f, 1.0f, 0.4f, 0.0f};
//        float Ambient[4] = {1.0f, 0.0f, 0.0f, 1.0f};
//        glLightfv(GL_LIGHT0, GL_POSITION, LightPos2);
//        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
    }

    void Renderer::WallRenderer::afterRender() {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_BLEND);
        glUseProgram(0);
    }
} // Renderer