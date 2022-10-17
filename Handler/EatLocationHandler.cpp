#include "EatLocationHandler.h"

namespace Handler {
    EatLocationHandler::EatLocationHandler(Barriers* barriers, Snake *snake, Eat *eat) : barriers(barriers), eat(eat), snake(snake) {
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
            cout << "Eat: X=" << newPos.x << ", Y=" << newPos.y << endl;
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
            if ((int) (*Iter)->tile->getPosition().x - 16 + 32 >= posX && (int) (*Iter)->tile->getPosition().x - 16 <= posX
                && (int) (*Iter)->tile->getPosition().y - 16 + 32 >= posY && (int) (*Iter)->tile->getPosition().y - 16 <= posY) {
                cout << "Not empty field" << endl;

                return false;
            }
        }

        for (auto Iter = barriers->getItems().begin(); Iter < barriers->getItems().end(); Iter++) {
            if ((int) (*Iter)->getPosition().x - 16 + 32 > posX && (int) (*Iter)->getPosition().x - 16 <= posX
                && (int) (*Iter)->getPosition().y - 16 + 32 > posY && (int) (*Iter)->getPosition().y - 16 <= posY) {
                cout << "Not empty field" << endl;

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

        cout << "X: " << numberX << endl;
        cout << "Y: " << numberY << endl;

        // tady musim checknout, ze je to policko prazdne, jinak musim najit jine
        glm::vec3 pos = eat->getPosition();
        if (isFieldEmpty(numberX, numberY)) {
            glm::vec2 pos = {numberX, numberY};

            return pos;
        }

        throw std::invalid_argument("");
    }

    void EatLocationHandler::onCheckPlaceHandler() {
        if (!eat->isVisible()) {
            rePosition();
        }
    }

    void EatLocationHandler::addTile() {
        snake->addTile((*snake->getItems().begin())->direction);
        snake->addTile((*snake->getItems().begin())->direction);
    }

} // Handler