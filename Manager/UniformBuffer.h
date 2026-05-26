#ifndef SNAKE3_UNIFORMBUFFER_H
#define SNAKE3_UNIFORMBUFFER_H

#include <cstddef>

#include "../stdafx.h"

namespace Manager {

    // Generic GL Uniform Buffer Object wrapper. Allocate fixed-size storage
    // once, upload data per frame (or on change), and bind to an indexed
    // binding point so shaders with `layout(std140, binding=N)` consume it.
    //
    // Per-frame data uses GL_STREAM_DRAW (driver hint: rewritten every frame).
    // Per-material data set once uses GL_STATIC_DRAW.
    class UniformBuffer {
    public:
        UniformBuffer() = default;
        ~UniformBuffer();

        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;

        UniformBuffer(UniformBuffer&& other) noexcept;
        UniformBuffer& operator=(UniformBuffer&& other) noexcept;

        void allocate(std::size_t bytes, GLenum usage = GL_STREAM_DRAW);

        void upload(const void* data, std::size_t bytes, std::size_t offset = 0) const;

        // glBindBufferBase to GL_UNIFORM_BUFFER target at index `bindingPoint`.
        // Shader-side `layout(std140, binding=N)` reads from matching index.
        void bindTo(GLuint bindingPoint) const;

        [[nodiscard]] GLuint id() const { return ubo; }
        [[nodiscard]] std::size_t size() const { return bytes; }

    private:
        GLuint ubo = 0;
        std::size_t bytes = 0;
    };

} // namespace Manager

#endif //SNAKE3_UNIFORMBUFFER_H
