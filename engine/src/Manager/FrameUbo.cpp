#include <snake3d/Manager/FrameUbo.h>

namespace Manager {

    void FrameUbo::init() {
        ubo.allocate(sizeof(FrameData), GL_STREAM_DRAW);
    }

    void FrameUbo::upload(const FrameData& data) {
        // Lazy init: App constructs RenderManager at static-init time (before
        // glewInit), so glGenBuffers in init() would crash. First per-frame
        // upload runs after GLEW is ready - allocate here.
        if (ubo.id() == 0) {
            init();
        }
        ubo.upload(&data, sizeof(FrameData));
    }

    void FrameUbo::bind() const {
        ubo.bindTo(UBO_BINDING_FRAME);
    }

} // namespace Manager
