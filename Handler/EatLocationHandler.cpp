#include "EatLocationHandler.h"

namespace Handler {
    EatLocationHandler::EatLocationHandler(Barriers* barriers, Snake *snake, Eat *eat, Radar* radar) :
        barriers(barriers), eat(eat), snake(snake), counter(0), radar(radar) {
    }

    EatLocationHandler::~EatLocationHandler() = default;

    void EatLocationHandler::onDefaultHandler() {
        // had sezral zradlo, je nutne repozicovat zradlo a zvetsit hada
        addTile();
        rePosition();
    }

    bool EatLocationHandler::rePosition() {
        try {
            glm::vec2 newPos = getPosition();
            glm::vec3 pos = eat->getPosition();
            //cout << "Eat: X=" << newPos.x << ", Y=" << newPos.y << endl;
            eat->setVirtualX((int)newPos.x * 32 + 16);
            eat->setVirtualY((int)newPos.y * 32 + 16);
            eat->setPosition({-69 + (newPos.x * 6), -69 + (newPos.y * 6), pos.z});
            eat->setVisible(true);

            return true;
        } catch (const std::invalid_argument &e) {
            eat->setVisible(false);

            return false;
        }
    }

    bool EatLocationHandler::isFieldEmpty(int x, int y) {

        int posX = x * 32;
        int posY = y * 32;

        for (auto Iter = snake->getItems().begin(); Iter < snake->getItems().end(); Iter++) {
            if ((int) (*Iter)->tile->getVirtualX() - 16 + 32 >= posX && (int) (*Iter)->tile->getVirtualX() - 16 <= posX
                && (int) (*Iter)->tile->getVirtualY() - 16 + 32 >= posY && (int) (*Iter)->tile->getVirtualY() - 16 <= posY) {
                return false;
            }
        }

        for (auto Iter = barriers->getItems().begin(); Iter < barriers->getItems().end(); Iter++) {
            if ((int) (*Iter)->getVirtualX() - 16 + 32 > posX && (int) (*Iter)->getVirtualX() - 16 <= posX
                && (int) (*Iter)->getVirtualY() - 16 + 32 > posY && (int) (*Iter)->getVirtualY() - 16 <= posY) {
                return false;
            }
        }

        return true;
    }

    void EatLocationHandler::onFirstPlaceHandler() {
        while (true) {
            try {
                glm::vec2 newPos = getPosition();
                glm::vec3 pos = eat->getPosition();
                eat->setVirtualX((int)newPos.x * 32 + 16);
                eat->setVirtualY((int)newPos.y * 32 + 16);
                eat->setPosition({-69 + (newPos.x * 6), -69 + (newPos.y * 6), pos.z});
                eat->setVisible(true);
                break;
            } catch (const std::invalid_argument &e) {
            }
        }
    }

    glm::vec2 EatLocationHandler::getPosition() {

        random_device rd; // obtain a random number from hardware
        mt19937 gen(rd()); // seed the generator
        uniform_int_distribution<> fields(0, 47); // define the range

        int numberX = fields(gen);
        int numberY = fields(gen);

        // tady musim checknout, ze je to policko prazdne, jinak musim najit jine
        glm::vec3 pos = eat->getPosition();
        if (isFieldEmpty(numberX, numberY)) {
            glm::vec2 pos = {numberX, numberY};

            return pos;
        }

        throw std::invalid_argument("");
    }

    void EatLocationHandler::onCheckPlaceHandler() {
        if (!eat->isVisible() && (*snake->getItems().begin())->direction > STOP && (*snake->getItems().begin())->direction < CRASH) {
            rePosition();
        }
    }

    void EatLocationHandler::addTile() {
        counter++;

        for(int x = 0; x < counter + 1; x++) {
            auto tile = snake->addTile((*snake->getItems().begin())->direction);
            if (tile != nullptr) {
                radar->addItem(tile->tile, {0.278, 1., 0.});
            }
        }
    }

    void EatLocationHandler::onCleanHandler() {
        counter = 0;
    }

} // Handler