#ifndef LIGHTNINGFLASHEFFECT_H
#define LIGHTNINGFLASHEFFECT_H

#include <queue>
#include <GL/glew.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/ShaderManager.h"

using namespace Manager;

namespace Effects {

class LightningFlashEffect {
public:
    LightningFlashEffect(Camera* camera, glm::mat4 proj, ResourceManager* resManager);
    void init();
    void triggerSequence(const std::vector<float>& pulses, float pulseDuration);
    void update(float deltaTime);
    void render() const;
    bool isActive() const;

private:
    float alpha = 0.0f;
    float pulseDuration = 0.1f;
    float timer = 0.0f;
    bool flashing = false;

    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    Camera* camera;
    glm::mat4 projection{};
    ResourceManager* resourceManager;
    ShaderManager* flashShader;
    std::queue<float> pulseQueue;

    void setupFullscreenQuad();
    void nextPulse();
};

} // Effects

#endif //LIGHTNINGFLASHEFFECT_H
