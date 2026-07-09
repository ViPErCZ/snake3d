#include <snake3d/Renderer/Opengl/Material/Uniform/CallbackUniform.h>

#include <utility>

using namespace std;
using namespace Manager;
using namespace Material;

namespace Uniform {
    CallbackUniform::CallbackUniform(CallbackType callback) : callback(std::move(callback)) {
    }

    void CallbackUniform::bind(const shared_ptr<ShaderProgram> &shader, const string &name) {
        callback(name, shader);
    }

    shared_ptr<IUniform> CallbackUniform::clone() const {
        auto cloned = make_shared<CallbackUniform>(callback);

        return cloned;
    }
} // Uniform
