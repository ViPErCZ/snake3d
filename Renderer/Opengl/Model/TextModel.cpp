#include "TextModel.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

namespace Model {
    TextModel::TextModel(unsigned int width, unsigned int height, ShaderManager *shader) : shader(shader) {
        this->shader = shader;
        this->shader->use();
        this->shader->setMat4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f));
        this->shader->setInt("text", 0);
        // configure VAO/VBO for texture quads
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    TextModel::~TextModel() {
        delete text;
        for (auto c : characters)
        {
            delete c.second;
        }

        characters.clear();
    }

    void TextModel::load(Text* text) {
        this->text = text;
        // first clear the previously loaded Characters
        characters.clear();
        // then initialize and load the FreeType library
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        // load font as face
        FT_Face face;
        if (FT_New_Face(ft, text->getFontPath().c_str(), 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, text->getFontSize());
        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // then for the first 128 ASCII characters, pre-load/compile their characters and store them
        for (GLubyte c = 0; c < 128; c++) // lol see what I did there
        {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    (int)face->glyph->bitmap.width,
                    (int)face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // now store character for later use
            auto* character = new Character();
            character->setTextureId(texture);
            character->setSize(glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows));
            character->setBearing(glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top));
            character->setAdvance(static_cast<unsigned int>(face->glyph->advance.x));
            characters.insert(std::pair<char, Character*>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void TextModel::render() {
        // activate corresponding render state
        shader->use();
        shader->setVec3("textColor", text->getColor());
        shader->setFloat("alpha", text->getAlpha());

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(this->VAO);

        float x = text->getPosition().x;
        float y = text->getPosition().y;
        float scale = text->getZoom().x;
        for (char c : text->getText())
        {
            Character* ch = characters[c];

            float xpos = x + (float)ch->getBearing().x * scale;
            float ypos = y + (float)(characters['H']->getBearing().y - ch->getBearing().y) * scale;

            float w = (float)ch->getSize().x * scale;
            float h = (float)ch->getSize().y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                    { xpos,     ypos + h,   0.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 0.0f },
                    { xpos,     ypos,       0.0f, 0.0f },

                    { xpos,     ypos + h,   0.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch->getTextureId());
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph
            x += (float)(ch->getAdvance() >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Text *TextModel::getText() const {
        return text;
    }

} // Model