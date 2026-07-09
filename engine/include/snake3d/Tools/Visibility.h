#ifndef SNAKE3_VISIBILITY_H
#define SNAKE3_VISIBILITY_H

namespace Node3D {
    class Visibility {
    public:
        virtual ~Visibility() = default;

        [[nodiscard]] bool isVisible() const;

        // Gameplay/snapshot path. No-op když debug lock je aktivní -- ImGui
        // Inspector může vybraný objekt ručně toggle bez aby ho net snapshot
        // apply / EatLocationHandler / scéna přepsala každý frame zpět.
        void setVisible(bool visible);

        // Inspector-only API: enforce visibility regardless of gameplay code.
        // Lock zůstane aktivní dokud někdo nevolá unlockVisible() (typically
        // při deselect v inspectoru).
        void setVisibleForced(bool visible);
        void unlockVisible();
        [[nodiscard]] bool isVisibilityLocked() const { return debugLock; }

    protected:
        bool visible = true;
        bool debugLock = false;
    };
} // Node3D

#endif //SNAKE3_VISIBILITY_H
