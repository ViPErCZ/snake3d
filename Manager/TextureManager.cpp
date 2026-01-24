#include <GL/glew.h>
#include "TextureManager.h"

#include "../Resource/TextureLoader.h"

namespace Manager {
    TextureManager::TextureManager(const unsigned int id) {
        textures.push_back(id);
    }

    TextureManager::TextureManager(const aiTexel *buffer, const unsigned int size) {
        this->buffer.resize(size);
        std::memcpy(this->buffer.data(), buffer, size);
    }

    TextureManager::~TextureManager() {
        for (auto texture: textures) {
            glDeleteTextures(1, &texture);
        }

        textures.clear();
    }

    void TextureManager::bind() const {
        int index = 0;
        for (const auto texture: textures) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, texture);
            index++;
        }
    }

    void TextureManager::unbind() const {
        int index = 0;
        for (auto texture: textures) {
            unbind(index);
            index++;
        }
    }

    void TextureManager::lazyLoad(const bool isAlbedo = true) {
        if (textures.empty()) {
            addTexture(Resource::TextureLoader::bindFromBuffer(buffer, isAlbedo));
            buffer.clear();
        }
    }

    bool TextureManager::hasTexture() const {
        return !textures.empty();
    }

    void TextureManager::addTexture(const unsigned int id) {
        textures.push_back(id);
    }

    void TextureManager::bind(const int index, const int item) {
        glActiveTexture(GL_TEXTURE0 + index);
        const auto id = textures.begin() + item;
        if (id < textures.end()) {
            glBindTexture(GL_TEXTURE_2D, (*id));
        }
    }

    void TextureManager::bindArr(const int index, const int item) {
        glActiveTexture(GL_TEXTURE0 + index);
        const auto id = textures.begin() + item;
        if (id < textures.end()) {
            glBindTexture(GL_TEXTURE_2D_ARRAY, (*id));
        }
    }

    void TextureManager::unbind(const int index) const {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void TextureManager::cubeBind(const int index) const {
        glActiveTexture(GL_TEXTURE0 + index);
        const auto id = textures.begin();
        glBindTexture(GL_TEXTURE_CUBE_MAP, (*id));
    }
} // Manager