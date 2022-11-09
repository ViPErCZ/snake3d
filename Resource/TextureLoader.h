#ifndef SNAKE3_TEXTURELOADER_H
#define SNAKE3_TEXTURELOADER_H

#include "../stdafx.h"
#include <filesystem>
#include <memory>
#include <iostream>
#include <vector>

using namespace std;

namespace fs = std::filesystem;

namespace Resource {

    class TextureLoader {
    public:
        static unsigned int loadTexture(const fs::path& path);
        static unsigned int loadSkyboxTexture(vector<std::string> faces);
    };

} // Resource

#endif //SNAKE3_TEXTURELOADER_H
