#ifndef SNAKE3_VISIBILITY_H
#define SNAKE3_VISIBILITY_H

namespace Node3D {
    class Visibility {
    public:
        [[nodiscard]] bool isVisible() const;

        void setVisible(bool visible);

    protected:
        bool visible = true;
    };
} // Node3D

#endif //SNAKE3_VISIBILITY_H
