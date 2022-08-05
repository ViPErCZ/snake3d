#include "GameField.h"

namespace ItemsDto {
    GameField::~GameField() {
        Release();
    }

    void GameField::Release() {
        for (auto Iter = tile.begin(); Iter < tile.end(); Iter++) {
            delete (*Iter);
        }

        tile.clear();
    }

    void GameField::Init() {
        Cube* field = new Cube();
        field->addTexture(5);
        field->setPosition({0, 0, 0.0});
        field->setZoom({1, 1, 0.0});
        field->setVisible(true);
        field->setWidth(768);
        field->setHeight(768);

        tile.push_back(field);

        field = new Cube();
        field->addTexture(5);
        field->setPosition({768, 0, 0.0});
        field->setZoom({1, 1, 0.0});
        field->setVisible(true);
        field->setWidth(768);
        field->setHeight(768);

        tile.push_back(field);

        field = new Cube();
        field->addTexture(5);
        field->setPosition({0, 768, 0.0});
        field->setZoom({1, 1, 0.0});
        field->setVisible(true);
        field->setWidth(768);
        field->setHeight(768);

        tile.push_back(field);

        field = new Cube();
        field->addTexture(5);
        field->setPosition({768, 768, 0.0});
        field->setZoom({1, 1, 0.0});
        field->setVisible(true);
        field->setWidth(768);
        field->setHeight(768);

        tile.push_back(field);
    }

    const vector<Cube *> &GameField::getTiles() const {
        return tile;
    }
} // ItemsDto