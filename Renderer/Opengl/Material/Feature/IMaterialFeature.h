#ifndef SNAKE3_IMATERIALFEATURE_H
#define SNAKE3_IMATERIALFEATURE_H

#include <map>
#include <memory>
#include <string>

#include "../../../../Manager/MaterialPlaceholder.h"
#include "../../../../Manager/ShaderFeature.h"
#include "../../../../Manager/ShaderProgram.h"
#include "../RenderContext.h"

using namespace Manager;
using namespace Material;

namespace Feature {
    class IMaterialFeature {
    public:
        virtual ~IMaterialFeature() = default;

        [[nodiscard]] virtual ShaderFeatureMask flag() const = 0;

        virtual void bind(ShaderProgram& shader,
                          const RenderContext& ctx) const = 0;

        virtual void unbind(ShaderProgram& shader) const { (void)shader; }

        [[nodiscard]] virtual std::shared_ptr<IMaterialFeature> clone() const = 0;

        // C3: typed slot enum místo stringového markeru - překlep typu
        // `@MATEIAL_FRAGMENT_PRE` chytne kompilátor. MaterialBuilder mapuje
        // slot na string marker při skladání ShaderHandle.
        [[nodiscard]] virtual std::map<Manager::MaterialSlot, std::string> snippetPaths() const {
            return {};
        }
    };
} // Feature

#endif //SNAKE3_IMATERIALFEATURE_H
