#ifndef SNAKE3_CALLBACKUNIFORM_H
#define SNAKE3_CALLBACKUNIFORM_H

#include <functional>

#include "../IUniform.h"

using namespace Material;
using namespace std;

namespace Uniform {
    class CallbackUniform : public IUniform {
    public:
        using CallbackType = std::function<void(const string& name, const shared_ptr<ShaderManager>& shader)>;

        explicit CallbackUniform(CallbackType callback);

        void bind(const shared_ptr<ShaderManager> &shader, const string &name) override;

        [[nodiscard]] shared_ptr<IUniform> clone() const override;

    protected:
        CallbackType callback;
    };
} // Uniform

#endif //SNAKE3_CALLBACKUNIFORM_H
