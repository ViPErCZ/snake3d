#ifndef SNAKE3_TAGED_H
#define SNAKE3_TAGED_H

#include <memory>
#include <string>
#include <set>

using namespace std;

namespace Node3D {
    class Tagged {
        public:
        void addTag(const string &tag);
        void removeTag(const string &tag);
        bool hasTag(string tag);
    private:
        set<shared_ptr<string> > tags;
    };
} // Node3D

#endif //SNAKE3_TAGED_H