#include "EatLocationHandler.h"
#include <random>
#include <cmath>

namespace Handler {
    EatLocationHandler::EatLocationHandler(const shared_ptr<MeshNode3D> &barriers, const shared_ptr<SnakeMeshNode3D> &snake,
                                           const shared_ptr<CoinMeshNode3D> &eat)
        : barriers(barriers), snake(snake), eat(eat), counter(0) {
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
            eat->x = static_cast<int>(newPos.x) * 32 + 16;
            eat->y = static_cast<int>(newPos.y) * 32 + 16;
            eat->setPosition({-69 + (newPos.x * 6), -69 + (newPos.y * 6), pos.z});
            eat->setVisible(true);
        } catch (const std::invalid_argument &e) {
            eat->setVisible(false);
        }
    }

    bool EatLocationHandler::isFieldEmpty(const int x, const int y) const {
        const int posX = x * 32;
        const int posY = y * 32;
        const float worldX = -25.0f + (static_cast<float>(x + 1) * 2.0f);
        const float worldY = -25.0f + (static_cast<float>(y + 1) * 2.0f);

        const auto isSameFieldAsWorldPos = [worldX, worldY](const shared_ptr<MeshNode3D> &node) {
            constexpr float eps = 0.0001f;
            const auto nodePos = node->getPosition();
            return std::abs(nodePos.x - worldX) < eps && std::abs(nodePos.y - worldY) < eps;
        };

        if (snake->x - 16 + 32 >= posX && snake->x - 16 <= posX
            && snake->y - 16 + 32 >= posY && snake->y - 16 <= posY) {
            return false;
        }

        for (auto Iter = snake->getChildren().begin(); Iter < snake->getChildren().end(); ++Iter) {
            if ((*Iter)->x - 16 + 32 >= posX && (*Iter)->x - 16 <= posX
                && (*Iter)->y - 16 + 32 >= posY && (*Iter)->y - 16 <= posY) {
                return false;
            }
        }

        if (isSameFieldAsWorldPos(barriers)) {
            return false;
        }
        if (barriers->x == posX + 32 && barriers->y == posY + 32) {
            return false;
        }

        for (auto Iter = barriers->getChildren().begin(); Iter < barriers->getChildren().end(); ++Iter) {
            if (isSameFieldAsWorldPos(*Iter)) {
                return false;
            }
            if ((*Iter)->x == posX + 32 && (*Iter)->y == posY + 32) {
                return false;
            }
        }

        return true;
    }

    void EatLocationHandler::clearBarriers() {
        barriers = nullptr;
    }

    void EatLocationHandler::onFirstPlaceHandler() const {
        while (true) {
            try {
                const glm::vec2 newPos = getPosition();
                glm::vec3 pos = eat->getPosition();
                eat->x = static_cast<int>(newPos.x) * 32 + 16;
                eat->y = static_cast<int>(newPos.y) * 32 + 16;
                eat->setPosition({-69 + (newPos.x * 6), -69 + (newPos.y * 6), pos.z});
                eat->setScale({0.013888889, 0.013888889, 0.013888889});
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
        if (!eat->isVisible() &&
            snake->getDirection() > SnakeMeshNode3D::STOP && snake->getDirection() < SnakeMeshNode3D::CRASH) {
            rePosition();
        }
    }

    void EatLocationHandler::addTile() {
        counter++;

        for (int x = 0; x < counter + 1; x++) {
            snake->addTile(snake->getDirection());
        }
    }

    void EatLocationHandler::onCleanHandler() {
        counter = 0;
    }
} // Handler
