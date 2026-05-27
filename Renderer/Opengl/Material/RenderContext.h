#ifndef SNAKE3_RENDERCONTEXT_H
#define SNAKE3_RENDERCONTEXT_H

#include <glm/glm.hpp>

namespace Manager {
    struct MaterialDataStd140; // forward decl; full type in Manager/MaterialUbo.h
}

namespace Material {
    struct RenderContext {
        glm::vec3 viewPos{0.0f};
        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};
        glm::mat4 model{1.0f};
        float uTime = 0.0f;
        bool shadows = false;
        mutable Manager::MaterialDataStd140* materialData = nullptr;
        mutable bool* materialDirty = nullptr;
    };
} // Material

#endif //SNAKE3_RENDERCONTEXT_H
