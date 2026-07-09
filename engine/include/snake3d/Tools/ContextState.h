#ifndef SNAKE3_CONTEXTSTATE_H
#define SNAKE3_CONTEXTSTATE_H

#include <unordered_map>

#include <snake3d/Tools/Blending.h>
#include <snake3d/Tools/Capabilities.h>
#include <snake3d/Tools/BlendFactor.h>
#include <snake3d/Tools/DepthFunc.h>

namespace Tools {
    class ContextState {
    public:
        ContextState();
        ~ContextState() = default;

        void disable(Capabilities func) noexcept;
        void enable(Capabilities func) noexcept;
        void setBlendingMode(Blending blending) noexcept;
        void setDepthTest(bool depthTest);
        void setDepthWrite(bool depthWrite);
        void setBlendFunc(BlendFactor src, BlendFactor dst) noexcept;
        void setDepthFunc(DepthFunc func) noexcept;
        void setDepthMask(DepthMask mask) noexcept;
    protected:
        void init() noexcept;
        std::unordered_map<Capabilities, bool> capability_map;
        BlendFactor src_factor {BlendFactor::None};
        BlendFactor dst_factor {BlendFactor::Zero};
        DepthFunc depth_func {DepthFunc::Less};
        DepthMask depth_mask {DepthMask::True};
    };
} // Tools

#endif //SNAKE3_CONTEXTSTATE_H