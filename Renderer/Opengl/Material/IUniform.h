#ifndef SNAKE3_IUNIFORM_H
#define SNAKE3_IUNIFORM_H

#include <memory>
#include "../../../Manager/ShaderManager.h"

using namespace std;
using namespace Manager;

namespace Material {
    class IUniform {
    public:
        virtual ~IUniform() = default;
        virtual void bind(const shared_ptr<ShaderManager>& shader, const string& name) = 0;
    };
} // Material

#endif //SNAKE3_IUNIFORM_H