#ifndef SNAKE3_TEXTURELOADER_H
#define SNAKE3_TEXTURELOADER_H

#include <filesystem>
#include <memory>
#include <vector>

#include <snake3d/Manager/TextureManager.h>

namespace fs = std::filesystem;

namespace Resource {
    class TextureLoader {
    public:
        static unsigned int loadTexture(const fs::path& path, bool isAlbedo = true);
        static unsigned int bindFromBuffer(const std::vector<unsigned char> &buffer, bool isAlbedo = true);
        static unsigned int bindFromBuffer(const void* buffer, unsigned int length, bool isAlbedo = true);
        static std::vector<unsigned char> loadTextureToBuffer(const fs::path& path);
        static unsigned int loadSkyboxTexture(const std::vector<std::string> &faces);
        static unsigned int bindFromBufferWithoutDecode(const void *buffer, bool isAlbedo, int widthImg, int heightImg, int numColCh);
        static std::shared_ptr<Manager::TextureManager> decodeImage(const void * buffer, unsigned int length);
    };

} // Resource

#endif //SNAKE3_TEXTURELOADER_H
