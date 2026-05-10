#include "Font.h"
#include <iostream>
#include <utility>
#include <GL/glew.h>
#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H

namespace Material {
    Font::Font(std::string path, const int pixelSize)
        : fontSize(pixelSize), ascender_pixels(0), fontPath(std::move(path)) {
        buildAtlas();
    }

    Font::~Font() {
        if (atlasTexture)
            glDeleteTextures(1, &atlasTexture);
    }

    float Font::getAscenderPixels() const {
        return ascender_pixels;
    }

    void Font::buildAtlas() {
        constexpr int atlasWidth = 1024;
        constexpr int atlasHeight = 1024;
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            std::cerr << "FT init fail\n";
            return;
        }
        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
            std::cerr << "FT load face fail\n";
            return;
        }
        FT_Set_Pixel_Sizes(face, 0, fontSize);

        std::vector<unsigned char> atlasData(atlasWidth * atlasHeight, 0);

        int x = 0, y = 0, rowH = 0;

        for (unsigned char c = 32; c < 128; c++) {
            if (FT_Load_Char(face, c,FT_LOAD_RENDER)) continue;
            const FT_Bitmap &bmp = face->glyph->bitmap;
            if (x + bmp.width >= atlasWidth) {
                x = 0;
                y += rowH;
                rowH = 0;
            }
            for (int row = 0; row < bmp.rows; row++)
                for (int col = 0; col < bmp.width; col++)
                    atlasData[(x + col) + (y + row) * atlasWidth] = bmp.buffer[col + row * bmp.width];

            Character ch{};
            ch.size = glm::ivec2(bmp.width, bmp.rows);
            ch.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            ch.advance = face->glyph->advance.x >> 6;
            ch.uvOffset = glm::vec2(static_cast<float>(x) / atlasWidth, static_cast<float>(y) / atlasHeight);
            ch.uvSize = glm::vec2(static_cast<float>(bmp.width) / atlasWidth, static_cast<float>(bmp.rows) / atlasHeight);
            characters.insert({static_cast<char>(c), ch});

            x += static_cast<int>(bmp.width);
            if (bmp.rows > rowH) rowH = static_cast<int>(bmp.rows);
        }

        ascender_pixels = static_cast<float>(face->size->metrics.ascender >> 6);
        ascender = face->ascender;
        descender = face->descender;
        lineHeight = face->height;
        scale = static_cast<float>(fontSize) / static_cast<float>(face->units_per_EM);
        ascender_pixels = lineHeight * scale;

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glGenTextures(1, &atlasTexture);
        glBindTexture(GL_TEXTURE_2D, atlasTexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RED, atlasWidth, atlasHeight, 0,GL_RED,GL_UNSIGNED_BYTE, atlasData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    const Character *Font::getCharacter(const char c) const {
        const auto it = characters.find(c);

        return it != characters.end() ? &it->second : nullptr;
    }

} // Material
