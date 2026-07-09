#ifndef SNAKE3_BASEMATERIAL_H
#define SNAKE3_BASEMATERIAL_H

#include <memory>

#include <snake3d/Renderer/Opengl/Material/Interface/BlendingInterface.h>

namespace Material {
    class BaseMaterial : public BlendingInterface {
    public:
        ~BaseMaterial() override = default;

        [[nodiscard]] virtual std::shared_ptr<BaseMaterial> clone() const = 0;

        // E2: polygon-offset depth bias applied around this material's draw. Negative pulls
        // the surface TOWARD the camera in depth-buffer space (no world-space move), so a
        // coplanar overlay (decal, waterline) wins the depth test without z-fighting and
        // without floating off silhouette edges. 0 = disabled (default). Units = GL polygon
        // offset factor/units (e.g. -1 .. -3 for decals).
        void setDepthBias(const float bias) { depthBias = bias; }
        [[nodiscard]] float getDepthBias() const { return depthBias; }

    protected:
        float depthBias = 0.0f;
    };
} // Material

#endif //SNAKE3_BASEMATERIAL_H
