#include "LevelManager.h"

namespace Manager {
    LevelManager::LevelManager(int level, int live, Barriers *barriers) : level(level), live(live),
                                                                          barriers(barriers), eatCounter(0) {}

    void LevelManager::setLevel(int level) {
        LevelManager::level = level;
    }

    void LevelManager::setLive(int live) {
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
                for (char & c : line)
                {
                    if (c == 49) { // "1"
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

    void LevelManager::setEatCounter(int eatCounter) {
        LevelManager::eatCounter = eatCounter;
    }

} // Manager