#ifndef SNAKE3_CONTEXTSTATE_H
#define SNAKE3_CONTEXTSTATE_H

#include <unordered_map>

#include "Blending.h"
#include "Capabilities.h"
#include "BlendFactor.h"

using namespace std;

namespace Tools {
    class ContextState {
    public:
        ContextState();
        ~ContextState() = default;

        void disable(Capabilities func) noexcept;
        void enable(Capabilities func) noexcept;
        void setBlendingMode(Blending blending) noexcept;
        void setBlendFunc(BlendFactor src, BlendFactor dst) noexcept;
    protected:
        void init() noexcept;
        unordered_map<Capabilities, bool> capability_map;
        BlendFactor src_factor {BlendFactor::None};
        BlendFactor dst_factor {BlendFactor::Zero};
    };
} // Tools

#endif //SNAKE3_CONTEXTSTATE_H