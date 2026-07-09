#ifndef SNAKE3_TAGED_H
#define SNAKE3_TAGED_H

#include <memory>
#include <string>
#include <set>

namespace Node3D {
    class Tagged {
        public:
        void addTag(const std::string &tag);
        void removeTag(const std::string &tag);
        bool hasTag(std::string tag);
    private:
        std::set<std::shared_ptr<std::string> > tags;
    };
} // Node3D

#endif //SNAKE3_TAGED_H