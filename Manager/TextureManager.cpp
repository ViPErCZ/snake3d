#include <GL/glew.h>
#include "TextureManager.h"

namespace Manager {

    TextureManager::~TextureManager() {
        for (auto texture: textures) {
            glDeleteTextures(1, &texture);
        }

        textures.clear();
    }

    void TextureManager::bind() const {
        int index = 0;
        for (auto texture: textures) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
    }

    void TextureManager::unbind() const {
        int index = 0;
        for (auto texture: textures) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void TextureManager::addTexture(unsigned int id) {
        textures.push_back(id);
    }

    void TextureManager::bind(int index, int item) {
        glActiveTexture(GL_TEXTURE0 + index);
        auto id = textures.begin() + item;
        if (id < textures.end()) {
            glBindTexture(GL_TEXTURE_2D, (*id));
        }
    }

} // Manager