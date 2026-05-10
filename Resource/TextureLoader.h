#ifndef SNAKE3_TEXTURELOADER_H
#define SNAKE3_TEXTURELOADER_H

#include <filesystem>
#include <vector>

#include "../Manager/EatManager.h"

using namespace std;

namespace fs = std::filesystem;

using namespace Manager;

namespace Resource {
    class TextureLoader {
    public:
        static unsigned int loadTexture(const fs::path& path, bool isAlbedo = true);
        static unsigned int bindFromBuffer(const vector<unsigned char> &buffer, bool isAlbedo = true);
        static unsigned int bindFromBuffer(const void* buffer, unsigned int length, bool isAlbedo = true);
        static vector<unsigned char> loadTextureToBuffer(const fs::path& path);
        static unsigned int loadSkyboxTexture(const vector<std::string> &faces);
        static unsigned int bindFromBufferWithoutDecode(const void *buffer, bool isAlbedo, int widthImg, int heightImg, int numColCh);
        static shared_ptr<TextureManager> decodeImage(const void * buffer, unsigned int length);
    };

} // Resource

#endif //SNAKE3_TEXTURELOADER_H
