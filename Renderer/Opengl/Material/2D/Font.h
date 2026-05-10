#ifndef SNAKE3_FONT_H
#define SNAKE3_FONT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

struct Character {
    glm::vec2 uvOffset; // levý dolní roh znaku v atlasu
    glm::vec2 uvSize; // velikost uv regionu (šířka/výška)
    glm::ivec2 size; // velikost bitmapy v pixelech
    glm::ivec2 bearing; // offset od baseline
    unsigned int advance; // vzdálenost pro další znak
};

namespace Material {
    class Font {
    public:
        explicit Font(std::string path, int pixelSize = 48);

        ~Font();

        const Character *getCharacter(char c) const;

        unsigned int getAtlasTextureId() const { return atlasTexture; }

        int getSize() const { return fontSize; }

        [[nodiscard]] float getAscenderPixels() const;

        float getAscenderPx() const   { return ascender * scale; }
        float getDescenderPx() const  { return descender * scale; }
        float getLineHeightPx() const { return lineHeight * scale; }

    private:
        void buildAtlas();

        std::unordered_map<char, Character> characters;
        unsigned int atlasTexture = 0;
        int fontSize;
        float ascender_pixels;
        float scale{};
        float ascender{};
        float descender{};
        float lineHeight{};
        std::string fontPath;
        std::vector<unsigned char> atlasBuffer;
    };
} // Material

#endif //SNAKE3_FONT_H
