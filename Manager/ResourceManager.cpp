#include "ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Thirdparty/stbimage/stb_image.h"

namespace Manager {
    ResourceManager::~ResourceManager() {
        Release();
    }

    bool ResourceManager::createTexture(const char* path, const string& name, GLint filter) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int widthImg, heightImg, numColCh;
        unsigned char *data = stbi_load(path, &widthImg, &heightImg, &numColCh, 0);
        if (data)
        {
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Check what type of color channels the baseColor has and load it accordingly
            if (numColCh == 4)
                glTexImage2D
                        (
                                GL_TEXTURE_2D,
                                0,
                                GL_RGBA,
                                widthImg,
                                heightImg,
                                0,
                                GL_RGBA,
                                GL_UNSIGNED_BYTE,
                                data
                        );
            else if (numColCh == 3)
                glTexImage2D
                        (
                                GL_TEXTURE_2D,
                                0,
                                GL_RGBA,
                                widthImg,
                                heightImg,
                                0,
                                GL_RGB,
                                GL_UNSIGNED_BYTE,
                                data
                        );
            else if (numColCh == 1)
                glTexImage2D
                        (
                                GL_TEXTURE_2D,
                                0,
                                GL_RGBA,
                                widthImg,
                                heightImg,
                                0,
                                GL_RED,
                                GL_UNSIGNED_BYTE,
                                data
                        );
            else {
                throw std::invalid_argument("Automatic Texture type recognition failed");
            }

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        this->textures.insert(std::pair<string, GLuint>(name, textureID));

        return true;
    }

    bool ResourceManager::Release() {
        for (auto & texture : textures) {
            glDeleteTextures(1, &texture.second);
        }

        model.clear();

        return true;
    }

    GLuint ResourceManager::getTexture(const string &name) {
        auto search = textures.find(name);
        if (search != textures.end()) {
            return search->second;
        }

        char buffer [50];
        sprintf(buffer, "texture with key %s not found", name.c_str());

        throw invalid_argument(buffer);
    }

    void ResourceManager::addModel(const string &name, std::shared_ptr<ObjItem> res) {
        std::unique_lock lock(mutex);
        const auto result = model.emplace(name, std::move(res));
        if (!result.second) {
            throw invalid_argument("Failed to add resource " + name + ", already contains.");
        }
    }

    ObjItem* ResourceManager::getModel(const string &name) {
        std::unique_lock lock(mutex);
        try {
            return model.at(name).get();
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

} // Manager