#ifndef SNAKE3_IUNIFORM_H
#define SNAKE3_IUNIFORM_H

#include <memory>
#include <string>
#include <snake3d/Manager/ShaderProgram.h>

namespace Material {
    class IUniform {
    public:
        virtual ~IUniform() = default;
        virtual void bind(const std::shared_ptr<Manager::ShaderProgram>& shader, const std::string& name) = 0;
        [[nodiscard]] virtual std::shared_ptr<IUniform> clone() const = 0;
    };
} // Material

#endif //SNAKE3_IUNIFORM_H