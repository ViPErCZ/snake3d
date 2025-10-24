#include "LevelManager.h"
#include <fstream>

namespace Manager {
    LevelManager::LevelManager(const int level, const int live, const shared_ptr<Barriers> &barriers)
        : level(level), live(live), eatCounter(0), barriers(barriers) {
    }

    void LevelManager::setLevel(const int level) {
        LevelManager::level = level;
    }

    void LevelManager::setLive(const int live) {
        LevelManager::live = live;
    }

    int LevelManager::getLevel() const {
        return level;
    }

    int LevelManager::getLive() const {
        return live;
    }

    void LevelManager::createLevel(int level) {
        this->level = level;
        this->eatCounter = 0;

        barriers->reset();
        string filename = "Assets/Levels/level";
        filename += std::to_string(level);
        filename += ".txt";

        ifstream infile(filename);
        if (infile.is_open()) {
            std::string line;
            int y = 0;
            while (std::getline(infile, line)) {
                int x = 0;
                for (char &c: line) {
                    if (c == 49) {
                        // "1"
                        barriers->createWall(-25 + ((x + 1) * 2), -25 + ((y + 1) * 2));
                    }

                    x++;
                }

                y++;
            }
            infile.close();
        }
    }

    int LevelManager::getEatCounter() const {
        return eatCounter;
    }

    void LevelManager::setEatCounter(const int eatCounter) {
        LevelManager::eatCounter = eatCounter;
    }
} // Manager
