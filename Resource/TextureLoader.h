#ifndef SNAKE3_TEXTURELOADER_H
#define SNAKE3_TEXTURELOADER_H

#include <filesystem>
#include <vector>

using namespace std;

namespace fs = std::filesystem;

namespace Resource {

    class TextureLoader {
    public:
        static unsigned int loadTexture(const fs::path& path, bool isAlbedo = true);
        static unsigned int bindFromBuffer(const vector<unsigned char> &buffer, bool isAlbedo = true);
        static unsigned int bindFromBuffer(const void* buffer, unsigned int length, bool isAlbedo = true);
        static vector<unsigned char> loadTextureToBuffer(const fs::path& path);
        static unsigned int loadSkyboxTexture(const vector<std::string> &faces);
    };

} // Resource

#endif //SNAKE3_TEXTURELOADER_H
