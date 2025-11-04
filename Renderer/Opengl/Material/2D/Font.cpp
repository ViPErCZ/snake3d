#include "Font.h"
#include <iostream>
#include <utility>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Material {
    Font::Font(std::string path, const int pixelSize)
        : fontSize(pixelSize), fontPath(std::move(path)) {
        buildAtlas();
    }

    Font::~Font() {
        if (atlasTexture)
            glDeleteTextures(1, &atlasTexture);
    }

    void Font::buildAtlas() {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            std::cerr << "ERROR::FREETYPE: Could not init FreeType Library\n";
            return;
        }

        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
            std::cerr << "ERROR::FREETYPE: Failed to load font: " << fontPath << "\n";
            FT_Done_FreeType(ft);
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        constexpr int atlasWidth = 1024;
        constexpr int atlasHeight = 1024;
        auto *atlasBuffer = new unsigned char[atlasWidth * atlasHeight];
        memset(atlasBuffer, 0, atlasWidth * atlasHeight);

        unsigned int xOffset = 0;
        unsigned int yOffset = 0;
        unsigned int rowHeight = 0;

        for (unsigned char c = 32; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cerr << "Failed to load Glyph " << c << "\n";
                continue;
            }

            const auto &bmp = face->glyph->bitmap;
            if (xOffset + bmp.width >= atlasWidth) {
                xOffset = 0;
                yOffset += rowHeight + 1;
                rowHeight = 0;
            }

            // zkopíruj glyph do atlasu
            for (int y = 0; y < bmp.rows; y++) {
                memcpy(
                    atlasBuffer + (yOffset + y) * atlasWidth + xOffset,
                    bmp.buffer + y * bmp.width,
                    bmp.width
                );
            }

            Character character{};
            character.uvOffset = glm::vec2(
                static_cast<float>(xOffset) / atlasWidth,
                static_cast<float>(yOffset) / atlasHeight
            );
            character.uvSize = glm::vec2(
                static_cast<float>(bmp.width) / atlasWidth,
                static_cast<float>(bmp.rows) / atlasHeight
            );
            character.size = glm::ivec2(bmp.width, bmp.rows);
            character.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            character.advance = static_cast<unsigned int>(face->glyph->advance.x);

            characters[static_cast<char>(c)] = character;

            xOffset += bmp.width + 1;
            if (bmp.rows > rowHeight)
                rowHeight = bmp.rows;
        }

        uploadAtlasToGPU(atlasBuffer, atlasWidth, atlasHeight);

        delete[] atlasBuffer;
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void Font::uploadAtlasToGPU(const unsigned char *buffer, const int width, const int height) {
        glGenTextures(1, &atlasTexture);
        glBindTexture(GL_TEXTURE_2D, atlasTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0,
                     GL_RED, GL_UNSIGNED_BYTE, buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    const Character *Font::getCharacter(const char c) const {
        const auto it = characters.find(c);

        return it != characters.end() ? &it->second : nullptr;
    }
} // Material
