#ifndef SNAKE3_NAMED_H
#define SNAKE3_NAMED_H

#include <string>

using namespace std;

namespace Node3D {
    class Named {
    public:
        void setName(const string &name);

        [[nodiscard]] string getName();

    protected:
        string name;
    };
} // Node3D

#endif //SNAKE3_NAMED_H