#include "EatLocationHandler.h"
#include <random>

namespace Handler {
    EatLocationHandler::EatLocationHandler(const shared_ptr<Barriers> &barriers, const shared_ptr<Snake> &snake,
                                           const shared_ptr<Eat> &eat,
                                           const shared_ptr<Radar> &radar)
        : radar(radar), snake(snake), barriers(barriers), eat(eat), counter(0) {
    }

    EatLocationHandler::~EatLocationHandler() = default;

    void EatLocationHandler::onDefaultHandler() {
        // had sezral zradlo, je nutne repozicovat zradlo a zvetsit hada
        addTile();
        rePosition();
    }

    void EatLocationHandler::rePosition() const {
        try {
            const glm::vec2 newPos = getPosition();
            glm::vec3 pos = eat->getPosition();
            //cout << "Eat: X=" << newPos.x << ", Y=" << newPos.y << endl;
            eat->setVirtualX(static_cast<int>(newPos.x) * 32 + 16);
            eat->setVirtualY(static_cast<int>(newPos.y) * 32 + 16);
            eat->setPosition({-69 + (newPos.x * 6), -69 + (newPos.y * 6), pos.z});
            eat->setVisible(true);
        } catch (const std::invalid_argument &e) {
            eat->setVisible(false);
        }
    }

    bool EatLocationHandler::isFieldEmpty(const int x, const int y) const {
        const int posX = x * 32;
        const int posY = y * 32;

        for (auto Iter = snake->getItems().begin(); Iter < snake->getItems().end(); ++Iter) {
            if ((*Iter)->tile->getVirtualX() - 16 + 32 >= posX && (*Iter)->tile->getVirtualX() - 16 <= posX
                && (*Iter)->tile->getVirtualY() - 16 + 32 >= posY && (*Iter)->tile->getVirtualY() - 16 <=
                posY) {
                return false;
            }
        }

        for (auto Iter = barriers->getItems().begin(); Iter < barriers->getItems().end(); ++Iter) {
            if ((*Iter)->getVirtualX() - 16 + 32 > posX && (*Iter)->getVirtualX() - 16 <= posX
                && (*Iter)->getVirtualY() - 16 + 32 > posY && (*Iter)->getVirtualY() - 16 <= posY) {
                return false;
            }
        }

        return true;
    }

    void EatLocationHandler::onFirstPlaceHandler() const {
        while (true) {
            try {
                const glm::vec2 newPos = getPosition();
                glm::vec3 pos = eat->getPosition();
                eat->setVirtualX(static_cast<int>(newPos.x) * 32 + 16);
                eat->setVirtualY(static_cast<int>(newPos.y) * 32 + 16);
                eat->setPosition({-69 + (newPos.x * 6), -69 + (newPos.y * 6), pos.z});
                eat->setZoom({0.013888889, 0.013888889, 0.013888889});
                eat->setVisible(true);
                break;
            } catch (const std::invalid_argument &e) {
            }
        }
    }

    glm::vec2 EatLocationHandler::getPosition() const {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> fields(0, 47);

        const int numberX = fields(gen);
        const int numberY = fields(gen);

        // tady musim checknout, ze je to policko prazdne, jinak musim najit jine
        if (isFieldEmpty(numberX, numberY)) {
            return {numberX, numberY};
        }

        throw std::invalid_argument("");
    }

    void EatLocationHandler::onCheckPlaceHandler() const {
        if (!eat->isVisible() && (*snake->getItems().begin())->direction > STOP && (*snake->getItems().begin())->
            direction < CRASH) {
            rePosition();
        }
    }

    void EatLocationHandler::addTile() {
        counter++;

        for (int x = 0; x < counter + 1; x++) {
            const auto tile = snake->addTile((*snake->getItems().begin())->direction);
            if (tile != nullptr) {
                radar->addItem(tile->tile, {0.278, 1., 0.});
            }
        }
    }

    void EatLocationHandler::onCleanHandler() {
        counter = 0;
    }
} // Handler
