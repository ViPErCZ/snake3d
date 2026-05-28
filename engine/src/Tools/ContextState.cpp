#include <snake3d/Tools/ContextState.h>

namespace Tools {
    ContextState::ContextState() {
        init();
    }

    void ContextState::setBlendingMode(const Blending blending) noexcept {
        switch (blending) {
            case Blending::Opaque:
                this->disable(Capabilities::Blending);
                break;
            case Blending::Additive:
                this->enable(Capabilities::Blending);
                this->setBlendFunc(BlendFactor::One, BlendFactor::One);
                break;
            case Blending::Modulate:
                this->enable(Capabilities::Blending);
                this->setBlendFunc(BlendFactor::DstColor, BlendFactor::Zero);
                break;
            case Blending::Translucent:
                this->enable(Capabilities::Blending);
                this->setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
                break;
            case Blending::AlphaAdditive:
                this->enable(Capabilities::Blending);
                this->setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::One);
                break;
            case Blending::Text:
                this->enable(Capabilities::Blending);
                this->setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
                break;
        }
    }

    void ContextState::setDepthTest(const bool depthTest) {
        if (depthTest) {
            this->enable(Capabilities::DepthTest);
            this->setDepthFunc(DepthFunc::Less);
            return;
        }
        this->disable(Capabilities::DepthTest);
    }

    void ContextState::setDepthWrite(const bool depthWrite) {
        this->setDepthMask(depthWrite ? DepthMask::True : DepthMask::False);
    }

    void ContextState::disable(Capabilities func) noexcept {
        if (capability_map[func]) {
            glDisable(static_cast<GLenum>(func));
            capability_map[func] = false;
        }
    }

    void ContextState::enable(Capabilities func) noexcept {
        if (!capability_map[func]) {
            glEnable(static_cast<GLenum>(func));
            capability_map[func] = true;
        }
    }

    void ContextState::setBlendFunc(const BlendFactor src, const BlendFactor dst) noexcept {
        if (src_factor != src || dst_factor != dst) {
            src_factor = src;
            dst_factor = dst;
            glBlendFunc(static_cast<GLenum>(src_factor), static_cast<GLenum>(dst_factor));
        }
    }

    void ContextState::setDepthFunc(DepthFunc func) noexcept {
        if (depth_func != func) {
            glDepthFunc(static_cast<GLenum>(func));
            depth_func = func;
        }
    }

    void ContextState::setDepthMask(DepthMask mask) noexcept {
        if (depth_mask != mask) {
            glDepthMask(static_cast<GLenum>(mask));
            depth_mask = mask;
        }
    }

    void ContextState::init() noexcept {
        capability_map.emplace(Capabilities::DepthTest, false);
        capability_map.emplace(Capabilities::Blending, false);
        capability_map.emplace(Capabilities::ProgramPointSize, false);
        capability_map.emplace(Capabilities::ScissorTest, false);
        capability_map.emplace(Capabilities::StencilTest, false);
        capability_map.emplace(Capabilities::CullFace, false);
    }
} // Tools
