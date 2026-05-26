#include "UniformBuffer.h"

namespace Manager {

    UniformBuffer::~UniformBuffer() {
        if (ubo != 0) {
            glDeleteBuffers(1, &ubo);
        }
    }

    UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
        : ubo(other.ubo), bytes(other.bytes) {
        other.ubo = 0;
        other.bytes = 0;
    }

    UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept {
        if (this != &other) {
            if (ubo != 0) {
                glDeleteBuffers(1, &ubo);
            }
            ubo = other.ubo;
            bytes = other.bytes;
            other.ubo = 0;
            other.bytes = 0;
        }
        return *this;
    }

    void UniformBuffer::allocate(const std::size_t newBytes, const GLenum usage) {
        if (ubo == 0) {
            glGenBuffers(1, &ubo);
        }
        bytes = newBytes;
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(bytes), nullptr, usage);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UniformBuffer::upload(const void* data, const std::size_t uploadBytes,
                               const std::size_t offset) const {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER,
                        static_cast<GLintptr>(offset),
                        static_cast<GLsizeiptr>(uploadBytes),
                        data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UniformBuffer::bindTo(const GLuint bindingPoint) const {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
    }

} // namespace Manager
