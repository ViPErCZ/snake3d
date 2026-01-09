#include "CallbackUniform.h"

#include <utility>

namespace Uniform {
    CallbackUniform::CallbackUniform(CallbackType callback) : callback(std::move(callback)) {
    }

    void CallbackUniform::bind(const shared_ptr<ShaderManager> &shader, const string &name) {
        callback(name, shader);
    }

    shared_ptr<IUniform> CallbackUniform::clone() const {
        auto cloned = make_shared<CallbackUniform>(callback);

        return cloned;
    }
} // Uniform
