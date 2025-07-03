#ifndef LIGHTNINGFLASHEFFECT_H
#define LIGHTNINGFLASHEFFECT_H

#include <GL/glew.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/ShaderManager.h"

using namespace Manager;

namespace Effects {

class LightningFlashEffect {
public:
    LightningFlashEffect(Camera* camera, glm::mat4 proj, ResourceManager* resManager);
    void init();
    void trigger();
    void update(float deltaTime);
    void render() const;
    bool isActive() const;

private:
    float alpha = 0.0f;
    float duration = 0.3f;
    float timer = 0.0f;
    bool flashing = false;

    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    Camera* camera;
    glm::mat4 projection{};
    ResourceManager* resourceManager;
    ShaderManager* flashShader;

    void setupFullscreenQuad();
};

} // Effects

#endif //LIGHTNINGFLASHEFFECT_H
