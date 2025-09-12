#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../Thirdparty/stbimage/stb_image.h"

namespace Resource {
    unsigned int TextureLoader::loadTexture(const fs::path &path, const bool isAlbedo) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int widthImg, heightImg, numColCh;
        unsigned char *data = stbi_load(path.c_str(), &widthImg, &heightImg, &numColCh, 0);
        if (data) {
            glBindTexture(GL_TEXTURE_2D, textureID);

            // // Check what type of color channels the baseColor has and load it accordingly
            // if (numColCh == 4)
            //     glTexImage2D
            //             (
            //                     GL_TEXTURE_2D,
            //                     0,
            //                     GL_RGBA,
            //                     widthImg,
            //                     heightImg,
            //                     0,
            //                     GL_RGBA,
            //                     GL_UNSIGNED_BYTE,
            //                     data
            //             );
            // else if (numColCh == 3)
            //     glTexImage2D
            //             (
            //                     GL_TEXTURE_2D,
            //                     0,
            //                     GL_RGBA,
            //                     widthImg,
            //                     heightImg,
            //                     0,
            //                     GL_RGB,
            //                     GL_UNSIGNED_BYTE,
            //                     data
            //             );
            // else if (numColCh == 1)
            //     glTexImage2D
            //             (
            //                     GL_TEXTURE_2D,
            //                     0,
            //                     GL_RGBA,
            //                     widthImg,
            //                     heightImg,
            //                     0,
            //                     GL_RED,
            //                     GL_UNSIGNED_BYTE,
            //                     data
            //             );
            // else {
            //     throw std::invalid_argument("Automatic Texture type recognition failed");
            // }
            if (isAlbedo) {
                // Albedo = barevná textura, musí být sRGB
                if (numColCh == 3)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                else if (numColCh == 4)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                else {
                    throw std::invalid_argument("Automatic Texture type recognition failed");
                }
            } else {
                // Metallic, roughness, ao, normal = data, lineární
                if (numColCh == 1)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                else if (numColCh == 3)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                else if (numColCh == 4)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                else {
                    throw std::invalid_argument("Automatic Texture type recognition failed");
                }
            }

            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    unsigned int TextureLoader::loadSkyboxTexture(vector<std::string> faces) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            } else {
                std::cout << "Texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
} // Resource