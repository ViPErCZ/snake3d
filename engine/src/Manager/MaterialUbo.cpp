#include <snake3d/Manager/MaterialUbo.h>

#include <snake3d/Manager/UboBindings.h>

namespace Manager {

    void MaterialUbo::upload(const MaterialDataStd140& data) {
        // Lazy init: MaterialInstance may be constructed before GLEW is
        // ready (resource loader runs on background threads). First upload
        // happens on the render thread post-init -> allocate here.
        // GL_STATIC_DRAW: per-material data changes rarely (driver hint).
        if (ubo.id() == 0) {
            ubo.allocate(sizeof(MaterialDataStd140), GL_STATIC_DRAW);
        }
        ubo.upload(&data, sizeof(MaterialDataStd140));
    }

    void MaterialUbo::bind() const {
        ubo.bindTo(UBO_BINDING_MATERIAL);
    }

} // namespace Manager
