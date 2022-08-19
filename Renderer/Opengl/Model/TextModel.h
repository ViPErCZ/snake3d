#ifndef SNAKE3_TEXTMODEL_H
#define SNAKE3_TEXTMODEL_H

#include "../../../Manager/ShaderManager.h"
#include "../../../ItemsDto/Text.h"
#include "Character.h"
#include <map>
#include <glm/gtc/matrix_transform.hpp>

using namespace Manager;
using namespace ItemsDto;
using namespace std;

namespace Model {

    class TextModel {
    public:
        virtual ~TextModel();
        explicit TextModel(unsigned int width, unsigned int height, ShaderManager *shader);
        void load(Text* text);
        void render();
        [[nodiscard]] Text *getText() const;

    protected:
        Text* text{};
        ShaderManager* shader;
        map<char, Character*> characters;
        unsigned int VAO{}, VBO{};
    };

} // Model

#endif //SNAKE3_TEXTMODEL_H
