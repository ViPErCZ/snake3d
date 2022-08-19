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
        Cube* field;

        field = new Cube();
        field->addTexture(5);
        field->setPosition({0.0f, 0.0f, 0.0f});
        field->setZoom({1, 1, 0.0});
        field->setVisible(true);

        tile.push_back(field);

        field = new Cube();
        field->addTexture(5);
        field->setPosition({2.0f, 0.0f, 0.0f});
        field->setZoom({1, 1, 0.0});
        field->setVisible(true);

        tile.push_back(field);

        field = new Cube();
        field->addTexture(5);
        field->setPosition({0.0f, 2.0f, 0.0f});
        field->setZoom({1, 1, 0.0});
        field->setVisible(true);

        tile.push_back(field);

        field = new Cube();
        field->addTexture(5);
        field->setPosition({2.0f, 2.0f, 0.0f});
        field->setZoom({1, 1, 0.0});
        field->setVisible(true);

        tile.push_back(field);
    }

    const vector<Cube *> &GameField::getTiles() const {
        return tile;
    }
} // ItemsDto