#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION

#include <fstream>

#include "../Thirdparty/stbimage/stb_image.h"

namespace Resource {
    unsigned int TextureLoader::loadTexture(const fs::path &path, const bool isAlbedo) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int widthImg, heightImg, numColCh;
        unsigned char *data = stbi_load(path.c_str(), &widthImg, &heightImg, &numColCh, 0);
        if (data) {
            glBindTexture(GL_TEXTURE_2D, textureID);
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
        } else {
            throw std::runtime_error(std::string("Texture failed to load at path: ") + path.c_str());
        }

        stbi_image_free(data);

        return textureID;
    }

    unsigned int TextureLoader::bindFromBuffer(const vector<unsigned char> &buffer, const bool isAlbedo) {
        unsigned int textureID;
        int widthImg, heightImg, numColCh;
        glGenTextures(1, &textureID);
        unsigned char* data = stbi_load_from_memory(
            buffer.data(),
            static_cast<int>(buffer.size()),
            &widthImg,
            &heightImg,
            &numColCh,
            0 // zachovej původní počet kanálů
        );
        if (data) {
            glBindTexture(GL_TEXTURE_2D, textureID);
            if (isAlbedo) {
                // Albedo = barevná textura, musí být sRGB
                if (numColCh == 3)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                else if (numColCh == 4)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE,data);
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
        } else {
            throw std::runtime_error("Texture failed to load from memory.");
        }

        stbi_image_free(data);

        return textureID;
    }

    vector<unsigned char> TextureLoader::loadTextureToBuffer(const fs::path &path) {
        // std::ifstream file(path, std::ios::binary | std::ios::ate);
        // const std::streamsize size = file.tellg();
        // file.seekg(0, std::ios::beg);

        std::vector<unsigned char> out;
        // file.read(reinterpret_cast<char*>(buffer.data()), size);

        namespace fs = std::filesystem;
        std::error_code ec;

        if (!fs::exists(path, ec)) {
            std::cerr << "[loadFile] file not found: " << path << " (" << ec.message() << ")\n";
            return out;
        }

        auto filesize = fs::file_size(path, ec);
        if (ec) {
            std::cerr << "[loadFile] file_size failed: " << ec.message() << ". Budu fallback číst streamem.\n";
            // fallback: přečíst přes stream iterator níže
        } else {
            if (filesize == 0) {
                std::cerr << "[loadFile] file is empty: " << path << "\n";
                return out;
            }
            // kontrola proti přetečení / příliš velkému požadavku
            if (filesize > std::vector<unsigned char>().max_size()) {
                std::cerr << "[loadFile] file is too large to fit into vector: " << filesize << "\n";
                return out;
            }

            // otevři a čti přesně 'filesize' bajtů
            std::ifstream ifs(path, std::ios::binary);
            if (!ifs) {
                std::cerr << "[loadFile] cannot open file: " << path << "\n";
                return out;
            }

            out.clear();
            out.resize(static_cast<size_t>(filesize));
            ifs.read(reinterpret_cast<char *>(out.data()), static_cast<std::streamsize>(filesize));
            if (!ifs) {
                // read může selhat
                std::cerr << "[loadFile] read failed (gcount=" << ifs.gcount() << ")\n";
                return out;
            }
        }

        // --- fallback: pokud file_size nebo jiné kroky selhaly, načti přes istreambuf_iterator
        std::ifstream ifs(path, std::ios::binary);
        if (!ifs) {
            std::cerr << "[loadFile] fallback: cannot open file: " << path << "\n";
            return out;
        }

        out.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

        if (out.empty()) {
            std::cerr << "[loadFile] fallback read produced empty buffer (maybe file empty or read error)\n";
            return out;
        }

        return out;
    }

    unsigned int TextureLoader::loadSkyboxTexture(const vector<std::string> &faces) {
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
            } else {
                std::cout << "Texture failed to load at path: " << faces[i] << std::endl;
            }
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
} // Resource