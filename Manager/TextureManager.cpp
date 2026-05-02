#include <GL/glew.h>
#include "TextureManager.h"

#include "../Resource/TextureLoader.h"

namespace Manager {
    TextureManager::TextureManager(const unsigned int id) : widthImg(0), heightImg(0), numColCh(0) {
        textures.push_back(id);
    }

    TextureManager::TextureManager(const aiTexel *buffer, const unsigned int size) : widthImg(0), heightImg(0),
                                                                                     numColCh(0) {
        this->buffer.resize(size);
        std::memcpy(this->buffer.data(), buffer, size);
    }

    TextureManager::TextureManager(const unsigned char *buffer, const unsigned int size, const unsigned int width,
                                   const unsigned int height, const unsigned int numColCh) : widthImg(width),
        heightImg(height), numColCh(numColCh) {
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
            addTexture(TextureLoader::bindFromBufferWithoutDecode(buffer.data(), isAlbedo, widthImg, heightImg, numColCh));
            buffer.clear();
        }
    }

    bool TextureManager::hasTexture() const {
        return !textures.empty();
    }

    void TextureManager::addTexture(const unsigned int id) {
        textures.push_back(id);
    }

    void TextureManager::replaceTexture(const unsigned int id, const int item) {
        if (item < 0) {
            return;
        }

        const auto index = static_cast<size_t>(item);
        if (index < textures.size()) {
            textures[index] = id;
            return;
        }

        if (index == textures.size()) {
            textures.push_back(id);
        }
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
